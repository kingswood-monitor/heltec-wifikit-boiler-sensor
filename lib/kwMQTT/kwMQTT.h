#ifndef H_KWMQTT
#define H_KWMQTT

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "globals.h"
#include "heltec.h"
#include <AsyncMqttClient.h>
#include <kwBoiler.h>

#define MQTT_HOST IPAddress(192, 168, 1, 1) // Mac Mini M1
#define MQTT_PORT 1883

#define DATA_COL 70
#define MAX_TOPIC_BUFFER_LEN 50

AsyncMqttClient mqttClient;

TimerHandle_t mqttReconnectTimer;
TimerHandle_t publishDataTimer;

const char *topicRoot = "kw_sensors";

char topicDataBoilerStateLED[MAX_TOPIC_BUFFER_LEN];
char topicDataBoilerCumulativeLED[MAX_TOPIC_BUFFER_LEN];
char topicMetaFirmware[MAX_TOPIC_BUFFER_LEN];
char topicMetaStatus[MAX_TOPIC_BUFFER_LEN];
char topicCommandRBE[MAX_TOPIC_BUFFER_LEN];

void connectToMqtt() {
  Serial.println("Connecting to MQTT");
  mqttClient.connect();
}

void publishData() 
{  
  char buf[10];
  cumulativeTimeSeconds(buf);
  
  if (getBoilerState() == 1)
  {
    mqttClient.publish(topicDataBoilerStateLED, 2, true, "1");
    Serial.printf("Boiler %s, cumulative %s seconds\n", "ON", buf);
    digitalWrite(LED, HIGH);
  }
  
  else {
    mqttClient.publish(topicDataBoilerStateLED, 2, true, "0");
    Serial.printf("Boiler %s, cumulative %s seconds\n", "OFF", buf);
    digitalWrite(LED, LOW);
  }
  
  mqttClient.publish(topicDataBoilerCumulativeLED, 2, true, buf);

  Heltec.display -> clear();
  Heltec.display -> drawString(0, 0, "Boiler:");
  Heltec.display -> drawString(50, 0, getBoilerState() == 1 ? "ON" : "OFF");
  Heltec.display -> drawString(0, 10, "Seconds:");
  Heltec.display -> drawString(50, 10, buf);
  Heltec.display -> display();
}

void onMqttConnect(bool sessionPresent) 
{
  Serial.println("Connected to MQTT");
  Heltec.display -> clear();
  delay(1000);
  Heltec.display -> display();

  mqttClient.publish(topicMetaFirmware, 2, true, g_firmwareVersion);
  mqttClient.publish(topicMetaStatus, 2, true, "ONLINE");

  // TODO: Add Command subscriptions
  
  xTimerStart(publishDataTimer, 0);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
  Serial.println("Disonnected from MQTT");
  Heltec.display -> clear();
  Heltec.display -> drawString(0, 0, "MQTT disconnected. Reconnecting...");
  Heltec.display -> display();
            
  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void onMqttPublish(uint16_t packetId)
{
  
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {}
void onMqttSubscribe(uint16_t packetId, uint8_t qos) {}
void onMqttUnsubscribe(uint16_t packetId) {}

void makeTopic(const char* type, const char* field, const char* g_sensorType, char* buf) 
{
  snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s/%s", topicRoot, type, field, g_sensorType, g_deviceID);
}

void makeTopic(const char* type, const char* field, char* buf) 
{
  snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s", topicRoot, type, field, g_deviceID);
}

void makeTopic(const char* type, char* buf) 
{
  snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s", topicRoot, type, g_deviceID);
}

#endif