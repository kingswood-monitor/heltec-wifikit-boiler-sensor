#include <Arduino.h>
#include <kwNeoTimer.h>
#include <kwHeltecWifikit32.h>
#include <kwBoiler.h>
#include <secrets.h>

#define SENSOR_TYPE "energy"
#define FIRMWARE_VERSION "2.1.0"
#define BOILER_SENSOR_PIN 27

uint8_t stateField;
uint8_t cumulativeSecsField;

kwHeltecWifikit32 heltec;
kwBoiler boiler{ BOILER_SENSOR_PIN };
kwNeoTimer publishDataTimer = kwNeoTimer();

HeltecConfig config;

void setup() 
{
    Serial.begin(115200);

    config.firmwareVersion = FIRMWARE_VERSION;
    config.ssid = WIFI_SSID;
    config.pwd = WIFI_PASSWORD;
    config.mqtt_host = IPAddress(192, 168, 1, 240); // Mac Mini M1
    config.topicRoot = "kw_sensors";
    config.rotateDisplay = true;

    stateField = heltec.registerField("State", "", "boilerState", "LED");
    cumulativeSecsField = heltec.registerField("On time", "secs", "boilerCumulativeSecs", "LED");
    
    publishDataTimer.set(1000);
    
    heltec.init(config);

    Serial.printf("\n------------------%s sensor------------------------------------------------\n\n", SENSOR_TYPE);
    Serial.printf("%-12s : %s\n", "Firmware", FIRMWARE_VERSION);
    Serial.printf("%-12s : %s\n", "Device ID", heltec.deviceID);
    Serial.printf("%-12s : %s %s\n", "RTC", heltec.hasRTC ? "OK" : "Not found", heltec.rtcWasAdjusted ? "[was not running - check time]" : "");
    Serial.printf("%-12s : %s\n", "Status", heltec.metaTopics[heltec.statusTopicID].c_str());
    for (int i=0; i < heltec.dataTopics.size(); i++)
    {
      Serial.printf("%-12s : %s\n", heltec.dataTopics[i].fieldName.c_str(), heltec.dataTopics[i].topicString.c_str());
    }
  }

int i = 0;
void loop() 
{  
  if (publishDataTimer.repeat())
  {
      if (heltec.isMidnight()) { boiler.resetActiveSeconds(); };

      heltec.publish(stateField, boiler.readState());
      heltec.publish(cumulativeSecsField, boiler.activeSeconds());
  }
  heltec.run();
}
