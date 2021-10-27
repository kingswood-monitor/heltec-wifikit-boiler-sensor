#ifndef H_KWMQTT
#define H_KWMQTT

extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#include "globals.h"
#include "heltec.h"
#include <AsyncMqttClient.h>

#define MQTT_HOST IPAddress(192, 168, 1, 1) // Mac Mini M1
#define MQTT_PORT 1883

#define DATA_COL 70

AsyncMqttClient mqttClient;

TimerHandle_t mqttReconnectTimer;
TimerHandle_t publishDataTimer;

#define MAX_TOPIC_BUFFER_LEN 50

const char *topicRoot = "kw_sensors";

char topicDataBoilerLED[MAX_TOPIC_BUFFER_LEN];
char topicMetaFirmware[MAX_TOPIC_BUFFER_LEN];
char topicMetaStatus[MAX_TOPIC_BUFFER_LEN];
char topicCommandRBE[MAX_TOPIC_BUFFER_LEN];

bool doRBE = false; 
bool state = false;
bool stateDidChange;
int oldState = HIGH;
bool isRising;
bool isFalling;

uint32_t cumulativeTimeMillis = 0;
uint32_t riseTimeMillis;
uint32_t onTimeMillis;

void connectToMqtt() {
  mqttClient.connect();
}

void publishData() 
{
    state = digitalRead(PIN_SENSOR);

    stateDidChange = (state != oldState);
    isRising = (oldState == false) && (state == true);
    isFalling = (oldState == true) && (state == false);

    oldState = state;

    if (isRising)
    {
      riseTimeMillis = xTaskGetTickCount();
    }
    else if (isFalling)
    {
      onTimeMillis = xTaskGetTickCount() - riseTimeMillis;
      cumulativeTimeMillis += onTimeMillis;
      Serial.printf("Was on %d seconds, cumulative %d seconds\n", onTimeMillis / 1000, cumulativeTimeMillis / 1000);
    }

    if (doRBE && !stateDidChange) {
      return;
    }
    
    if (state == HIGH)
    {
      mqttClient.publish(topicDataBoilerLED, 2, true, "1");
      digitalWrite(LED, HIGH);
    }
    else {
      mqttClient.publish(topicDataBoilerLED, 2, true, "0");
      digitalWrite(LED, LOW);
    }

    Serial.printf("Boiler %s millis %d\n", state == HIGH ? "ON" : "OFF", xTaskGetTickCount());
}

void onMqttConnect(bool sessionPresent) 
{
  mqttClient.publish(topicMetaFirmware, 2, true, g_firmwareVersion);
  mqttClient.publish(topicMetaStatus, 2, true, "ONLINE");

  // TODO: Add Command subscriptions
  
  xTimerStart(publishDataTimer, 0);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) 
{
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