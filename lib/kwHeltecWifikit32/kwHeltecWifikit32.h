#ifndef H_KWHELTECWIFIKIT32
#define H_KWHELTECWIFIKIT32

#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C

class kwHeltecWifikit32 
{
public:
  kwHeltecWifikit32();
  void initDisplay(int pin_rst, int pin_sda, int pin_scl);
  void initDisplay(int pin_rst, int pin_sda, int pin_scl, bool doRemap);
  bool initNetwork(const char* SSID, const char* PWD, IPAddress MQTT_HOST);
  void run();

private:
  void getMacAddress();
  static void mqttCallback(char* topic, byte* payload, unsigned int length);
  boolean mqttReconnect();

  char deviceID[16] = {0};
  long lastReconnectAttempt = 0;
};

#endif