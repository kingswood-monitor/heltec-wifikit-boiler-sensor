#include <Arduino.h>
#include <neotimer.h>
#include <kwHeltecWifikit32.h>
#include <kwBoiler.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "1.1.0"
#define BOILER_SENSOR_PIN 27

// OLED pins
#define PIN_RST 16
#define PIN_SDA 4
#define PIN_SCL 15

// Network credentials
#define WIFI_SSID "kingswoodguest"
#define WIFI_PASSWORD "wbtc0rar"
#define MQTT_HOST IPAddress(192, 168, 1, 1) // Mac Mini M1
#define MQTT_PORT 1883

// MQTT topics
char topicDataBoilerStateLED[MAX_TOPIC_BUFFER_LEN];
char topicDataBoilerCumulativeLED[MAX_TOPIC_BUFFER_LEN];
char topicCommandResetTimer[MAX_TOPIC_BUFFER_LEN];

Neotimer publishDataTimer = Neotimer();
kwHeltecWifikit32 heltec;
kwBoiler boiler{ BOILER_SENSOR_PIN };

void setup() 
{
    Serial.begin(115200);
    heltec.initDisplay(PIN_RST, PIN_SDA, PIN_SCL, true);
    heltec.initNetwork(WIFI_SSID, WIFI_PASSWORD, MQTT_HOST);

    heltec.makeTopic("data", "boilerState", "LED", topicDataBoilerStateLED);
    heltec.makeTopic("data", "boilerCumulative", "LED", topicDataBoilerCumulativeLED);
    heltec.makeTopic("command", "resetTimer", "LED", topicCommandResetTimer);
    
    Serial.printf("------------------%s sensor------------------\n", SENSOR_TYPE);
    Serial.printf("Firmware              : %s\n", FIRMWARE_VERSION);
    Serial.printf("Device ID             : %s\n", heltec.deviceID);
    Serial.printf("Sensor pin            : %d\n", boiler.sensorPin);
    Serial.printf("MQTT BoilerState      : %s\n", topicDataBoilerStateLED);
    Serial.printf("MQTT BoilerCumulative : %s\n", topicDataBoilerCumulativeLED);
    Serial.printf("MQTT Reset Timer      : %s\n", topicCommandResetTimer);
    Serial.printf("MQTT Status           : %s\n", heltec.topicMetaStatus);
    
    publishDataTimer.set(1000);

    // heltec.displayMode(DISPLAY_MODE_DATA);

  }

void loop() {
  if (publishDataTimer.repeat())
  {
    int boilerState = boiler.readState();
    heltec.publish(topicDataBoilerStateLED, boilerState);
    // heltec.display("State", boilerstate, 1);
  }
  
  heltec.run();
}
