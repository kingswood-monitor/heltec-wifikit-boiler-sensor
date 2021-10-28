#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <kwWiFi.h>
#include <kwMQTT.h>
#include <kwBoiler.h>

#define SENSOR_TYPE "energy"

// OLED library
#define I2C_ADDRESS 0x3C
#define RST_PIN 16
#define PIN_SDA 4
#define PIN_SCL 15


void getMacAddress(char* buf)
{
  String theAddress = WiFi.macAddress();
  theAddress.replace(":", "");
  strcpy(buf, theAddress.c_str());
}

void setup() 
{
    Serial.begin(115200);
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000L);

    oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
    oled.setFont(font5x7);
    oled.setScrollMode(SCROLL_MODE_AUTO);
    oled.displayRemap(true);

    getMacAddress(g_deviceID);

    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
    publishDataTimer = xTimerCreate("publishDataTimer", pdMS_TO_TICKS(g_sampleIntervalSeconds * 1000), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(publishData));
    readBoilerTimer = xTimerCreate("readBoilerTimer", pdMS_TO_TICKS(g_sampleIntervalSeconds * 500), pdTRUE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(readBoiler));

    makeTopic("data", "boilerState", "LED", topicDataBoilerStateLED);
    makeTopic("data", "boilerCumulative", "LED", topicDataBoilerCumulativeLED);
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

    oled.clear();
    oled.println(g_deviceID);
    oled.println(g_firmwareVersion);

    Serial.printf("------------------%s sensor------------------\n", SENSOR_TYPE);
    Serial.printf("Firmware              : %s\n", g_firmwareVersion);
    Serial.printf("Device ID             : %s\n", g_deviceID);
    Serial.printf("MQTT BoilerState      : %s\n", topicDataBoilerStateLED);
    Serial.printf("MQTT BoilerCumulative : %s\n", topicDataBoilerCumulativeLED);
    Serial.printf("MQTT Status           : %s\n", topicMetaStatus);
    Serial.printf("MQTT Firmware         : %s\n", topicMetaFirmware);
    Serial.printf("MQTT RBE              : %s\n", topicCommandRBE);
    
    xTimerStart(readBoilerTimer, 0);
    connectToWifi();

    while(1){;};
  }

void loop() {
  vTaskSuspend(NULL);
}