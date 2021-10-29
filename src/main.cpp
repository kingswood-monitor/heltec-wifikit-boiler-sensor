#include <Arduino.h>
#include <kwTimer.h>
#include <kwHeltecWifikit32.h>
#include <kwBoiler.h>
#include <secrets.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "1.1.0"
#define BOILER_SENSOR_PIN 27

// OLED pins
#define PIN_RST 16
#define PIN_SDA 4
#define PIN_SCL 15

// Network credentials
// #define WIFI_SSID xxx
// #define WIFI_PASSWORD xxx
#define MQTT_HOST IPAddress(192, 168, 1, 1) // Mac Mini M1
#define MQTT_PORT 1883

// MQTT topics
char topicDataBoilerStateLED[MAX_TOPIC_BUFFER_LEN];
char topicDataBoilerCumulativeLED[MAX_TOPIC_BUFFER_LEN];
char topicCommandResetTimer[MAX_TOPIC_BUFFER_LEN];

kwHeltecWifikit32 heltec;
kwBoiler boiler{ BOILER_SENSOR_PIN };
kwTimer publishDataTimer = kwTimer();

void setup() 
{
    Serial.begin(115200);
    heltec.initDisplay(PIN_RST, PIN_SDA, PIN_SCL, true);
    // heltec.initNetwork(WIFI_SSID, WIFI_PASSWORD, MQTT_HOST);

    heltec.makeTopic("data", "boilerState", "LED", topicDataBoilerStateLED);
    heltec.makeTopic("data", "boilerCumulativeSecs", "LED", topicDataBoilerCumulativeLED);
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

    delay(1000);
  }

void loop() {
  
  if (publishDataTimer.repeat())
  {
    char buf[20];
    
    // boiler state
    uint16_t boilerState = boiler.readState();
    heltec.publish(topicDataBoilerStateLED, boilerState);
    heltec.display((boilerState == 1) ? "HEATING" : "OFF", 0);
    
    // cumulative time
    uint16_t cumulativeSeconds = boiler.cumulativeSeconds();
    heltec.publish(topicDataBoilerCumulativeLED, cumulativeSeconds);

    if (cumulativeSeconds >= 60)
    {
      uint16_t cumulativeMinutes = cumulativeSeconds / 60;
      sprintf(buf, "%d minute%s", cumulativeMinutes, (cumulativeMinutes > 0) ? "s" : "");
    }
    else{
      sprintf(buf, "%d seconds", cumulativeSeconds);
    }
    
    heltec.display(buf, 6);
  }
  
  heltec.run();
}
