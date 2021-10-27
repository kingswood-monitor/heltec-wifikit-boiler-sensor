#ifndef H_KWWIFI
#define H_KWWIFI

#include "heltec.h"
#include "WiFi.h"
#include <kwMQTT.h>

#define WIFI_SSID "kingswoodpn"
#define WIFI_PASSWORD "wbtc0rar"

TimerHandle_t wifiReconnectTimer;

void connectToWifi() 
{  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void WiFiEvent(WiFiEvent_t event) 
{
    Heltec.display -> drawString(0, 30, "SSID:");
    Heltec.display -> drawString(COL2, 30, WIFI_SSID);
    Heltec.display -> drawString(0, 40, "IP:");
    Heltec.display -> display();

    switch(event) 
    {
        case SYSTEM_EVENT_STA_GOT_IP:
            Heltec.display -> drawString(COL2, 40, WiFi.localIP().toString());
            Heltec.display -> display();
            
            connectToMqtt();
            break;
        
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Heltec.display -> clear();
            Heltec.display -> drawString(0, 0, "WiFi disconnected. Reconnecting...");
            Heltec.display -> display();
            
            xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
            xTimerStop(publishDataTimer, 0);
            xTimerStart(wifiReconnectTimer, 0);
            
            digitalWrite(LED, LOW);
            break;
    }
}

#endif