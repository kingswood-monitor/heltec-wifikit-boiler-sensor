#include <Arduino.h>
#include <kwNeoTimer.h>
#include <kwHeltecWifikit32.h>
#include <kwBoiler.h>
#include <secrets.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "2.1.1"
#define BOILER_SENSOR_PIN 27
#define TOPIC_ROOT "kw_sensors"

uint8_t stateField;
uint8_t cumulativeSecsField;

struct HeltecConfig config = 
{
  .ssid = WIFI_SSID,
  .pwd = WIFI_PASSWORD,
  .mqtt_host = IPAddress(192, 168, 1, 240), // Mac Mini M1
  .rotateDisplay = true,
  .firmwareVersion = FIRMWARE_VERSION,
  .topicRoot = TOPIC_ROOT
};

kwHeltecWifikit32 heltec{ config };
kwBoiler boiler{ BOILER_SENSOR_PIN };
kwNeoTimer publishDataTimer = kwNeoTimer();

void setup() 
{
    Serial.begin(115200);

    stateField = heltec.registerField("State:", "", "boilerState", "LED");
    cumulativeSecsField = heltec.registerField("Active:", "mins", "boilerCumulativeSecs", "LED");

    heltec.init();

    Serial.printf("\n------------------%s sensor------------------------------------------------\n\n", SENSOR_TYPE);
    Serial.printf("%-12s : %s\n", "Firmware", FIRMWARE_VERSION);
    Serial.printf("%-12s : %s\n", "Device ID", heltec.deviceID);
    Serial.printf("%-12s : %s %s\n", "RTC", heltec.hasRTC ? "OK" : "Not found", heltec.rtcWasAdjusted ? "[was not running - check time]" : "");
    Serial.printf("%-12s : %s\n", "Status", heltec.metaTopics[heltec.statusTopicID].c_str());
    for (int i=0; i < heltec.dataTopics.size(); i++)
    {
      Serial.printf("%-12s : %s\n", heltec.dataTopics[i].fieldName.c_str(), heltec.dataTopics[i].topicString.c_str());
    }

    publishDataTimer.set(1000);
  }

void loop() 
{  
  if (publishDataTimer.repeat())
  {
      if (heltec.isMidnight()) { boiler.resetActiveSeconds(); };

      heltec.publish(stateField, boiler.readState());
      heltec.publish(cumulativeSecsField, boiler.activeSeconds());
      
      heltec.update(stateField, boiler.readState() ? "ON" : "OFF");
      heltec.update(cumulativeSecsField, (uint16_t)(boiler.activeSeconds() / 60.0));
  }
  heltec.run();
}
