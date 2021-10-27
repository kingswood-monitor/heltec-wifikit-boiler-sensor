#include <Arduino.h>
#include "heltec.h"
#include <kwWiFi.h>
#include <kwMQTT.h>

#define SENSOR_TYPE "energy"

void getMacAddress(char* buf)
{
  String theAddress = WiFi.macAddress();
  theAddress.replace(":", "");
  strcpy(buf, theAddress.c_str());
}

void setup() 
{
    Serial.begin(115200);
    Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Enable*/, true /*Serial Enable*/);
    getMacAddress(g_deviceID);

    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    publishDataTimer = xTimerCreate("publishDataTimer", pdMS_TO_TICKS(g_sampleIntervalSeconds * 1000), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(publishData));

    makeTopic("data", "boiler", "LED", topicDataBoilerLED);
    makeTopic("meta", "firmware", topicMetaFirmware);
    makeTopic("meta", "status", topicMetaStatus);
    makeTopic("command", "RBE", topicCommandRBE);

    WiFi.onEvent(WiFiEvent);

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setWill(topicMetaStatus, 2, 1, "OFFLINE", 7);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);

    Heltec.display -> clear();
    Heltec.display -> drawString(0, 0, "Device ID:");
    Heltec.display -> drawString(COL2, 0, g_deviceID);
    Heltec.display -> drawString(0, 10, "Firmware:");
    Heltec.display -> drawString(COL2, 10, g_firmwareVersion);
    Heltec.display -> display();
  
    Serial.printf("------------------%s sensor------------------\n", SENSOR_TYPE);
    Serial.printf("Firmware: %s\n", g_firmwareVersion);
    Serial.printf("Device ID: %s\n", g_deviceID);
    Serial.printf("MQTT Boiler   : %s\n", topicDataBoilerLED);
    Serial.printf("MQTT Status   : %s\n", topicMetaStatus);
    Serial.printf("MQTT Firmware : %s\n", topicMetaFirmware);
    Serial.printf("MQTT RBE      : %s\n", topicCommandRBE);
    
    connectToWifi();

  }

void loop() {
  
}