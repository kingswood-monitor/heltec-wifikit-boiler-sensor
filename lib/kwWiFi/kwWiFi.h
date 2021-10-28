#ifndef H_KWWIFI
#define H_KWWIFI

#include "heltec.h"
#include "WiFi.h"
#include <kwMQTT.h>

// #define WIFI_SSID "kingswoodpn"
#define WIFI_SSID "kingswoodguest"
// #define WIFI_SSID "Kingswood"
#define WIFI_PASSWORD "wbtc0rar"

TimerHandle_t wifiReconnectTimer;

void connectToWifi() 
{  
    Serial.printf("Connecting to WiFi, SSID %s\n", WIFI_SSID);
    Heltec.display -> drawString(0, 30, "SSID:");
    Heltec.display -> drawString(COL2, 30, WIFI_SSID);
    Heltec.display -> drawString(0, 40, "IP:");
    Heltec.display -> display();
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID);
}

void WiFiEvent(WiFiEvent_t event) 
{
    Serial.printf("WiFi event %d\n", event);
    switch(event) 
    {
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.printf("Got IP: %s\n", WiFi.localIP().toString());
            Heltec.display -> drawString(COL2, 40, WiFi.localIP().toString());
            Heltec.display -> display();
            
            connectToMqtt();
            break;
        
        case SYSTEM_EVENT_STA_START:
            Serial.println("Starting");
            break;
            
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to AP");
            break;
        
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Heltec.display -> clear();
            Heltec.display -> drawString(0, 0, "WiFi disconnected");
            Heltec.display -> drawString(0, 10, "Reconnecting...");
            Heltec.display -> display();
            
            xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
            xTimerStop(publishDataTimer, 0);
            xTimerStart(wifiReconnectTimer, 0);
            
            digitalWrite(LED, LOW);
            break;
    }
}

#endif