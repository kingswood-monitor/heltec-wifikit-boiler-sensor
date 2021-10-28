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
    oled.print("SSID: ");
    oled.println(WIFI_SSID);

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
            oled.println(WiFi.localIP().toString());    
            connectToMqtt();
            break;
        
        case SYSTEM_EVENT_STA_START:
            Serial.println("Starting");
            oled.println("Starting WiFi...");
            break;
            
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("Connected to AP");
            oled.println("Connected to AP...");
            break;
        
        case SYSTEM_EVENT_STA_DISCONNECTED:
            oled.clear();
            oled.println("WiFi disconnected");
            oled.println("Reconnecting...");
  
            xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
            xTimerStop(publishDataTimer, 0);
            xTimerStart(wifiReconnectTimer, 0);
            
            digitalWrite(LED, LOW);
            break;
    }
}

#endif