#ifndef H_KWHELTECWIFIKIT32
#define H_KWHELTECWIFIKIT32

#define MQTT_SOCKET_TIMEOUT 1

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C
#define MQTT_RECONNECT_TIME_SECONDS 5
#define MAX_TOPIC_BUFFER_LEN 50
#define TOPIC_ROOT "kw_sensors"


class kwHeltecWifikit32 
{
public:
  kwHeltecWifikit32();
  void initDisplay(int pin_rst, int pin_sda, int pin_scl);
  void initDisplay(int pin_rst, int pin_sda, int pin_scl, bool doRemap);
  bool initNetwork(const char* SSID, const char* PWD, IPAddress MQTT_HOST);
  void makeTopic(const char* type, const char* field, const char* sensorType, char* buf);
  void makeTopic(const char* type, const char* field, char* buf);
  void publish(char* topic, char* data);
  void publish(char* topic, uint16_t data);
  void publish(char* topic, float data);
  void display(const char* data, int row);
  void run();
  
  char deviceID[16] = {0};
  char topicMetaStatus[MAX_TOPIC_BUFFER_LEN];

private:
  void getMacAddress();
  void updateSystemStatus(std::string statusMessage);
  static void mqttCallback(char* topic, byte* payload, unsigned int length);
  boolean mqttReconnect();

  bool didInitialiseNetwork = false;
  long lastReconnectAttempt = 0;
  uint8_t maxRows;
  uint8_t maxCols;
};

#endif