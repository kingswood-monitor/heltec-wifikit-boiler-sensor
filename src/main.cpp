#include <Arduino.h>
#include <kwNeoTimer.h>
#include <kwHeltecWifikit32.h>
#include <kwBoiler.h>
#include <secrets.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "2.0.0"
#define BOILER_SENSOR_PIN 27

// OLED pins
#define PIN_RST 16
#define PIN_SDA 4
#define PIN_SCL 15

// Network credentials
// #define WIFI_SSID xxx
// #define WIFI_PASSWORD xxx
#define MQTT_HOST IPAddress(192, 168, 1, 240) // Mac Mini M1
#define MQTT_PORT 1883
#define TOPIC_ROOT "kw_sensors"

uint8_t stateField;
uint8_t cumulativeSecsField;

kwHeltecWifikit32 heltec;
kwBoiler boiler{ BOILER_SENSOR_PIN };
kwNeoTimer publishDataTimer = kwNeoTimer();

void setup() 
{
    Serial.begin(115200);
    heltec.initDisplay(PIN_RST, PIN_SDA, PIN_SCL, true);

    stateField = heltec.registerDataTopic("State", "", "boilerState", "LED");
    cumulativeSecsField = heltec.registerDataTopic("On time", "secs", "boilerCumulativeSecs", "LED");

    Serial.printf("\n------------------%s sensor------------------------------------------------\n\n", SENSOR_TYPE);
    Serial.printf("%-12s : %s\n", "Firmware", FIRMWARE_VERSION);
    Serial.printf("%-12s : %s\n", "Device ID", heltec.deviceID);
    for (int i=0; i < heltec.dataTopics.size(); i++)
    {
      Serial.printf("%-12s : %s\n", heltec.dataTopics[i].fieldName.c_str(), heltec.dataTopics[i].topicString.c_str());
    }
    Serial.printf("%-12s : %s\n", "status", heltec.metaTopics[heltec.statusTopicID].c_str());
  
    heltec.initWiFi(WIFI_SSID, WIFI_PASSWORD);
    heltec.initMTTQ(MQTT_HOST, TOPIC_ROOT);
    heltec.initTimeSync();

    publishDataTimer.set(1000);
  }

int i = 0;
void loop() 
{  
  if (publishDataTimer.repeat())
  {
      if (heltec.isMidnight()) { boiler.resetActiveSeconds(); };

      heltec.publish(stateField, boiler.readState());
      heltec.publish(cumulativeSecsField, boiler.activeSeconds());

      heltec.display();
  }
  heltec.run();
}
