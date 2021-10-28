#include <kwHeltecWifikit32.h>

SSD1306AsciiWire oled;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


// PUBLIC ///////////////////////////////////////////////////////////////////////

// kwHeltecWifikit32 constructor
kwHeltecWifikit32::kwHeltecWifikit32()
{
    getMacAddress();
    makeTopic("meta", "status", topicMetaStatus);
}

// Initalise the OLED display
void kwHeltecWifikit32::initDisplay(int pin_rst, int pin_sda, int pin_scl)
{
    initDisplay(pin_rst, pin_sda, pin_scl, false);
}

void kwHeltecWifikit32::initDisplay(int pin_rst, int pin_sda, int pin_scl, bool doRemap)
{
    Wire.begin(pin_sda, pin_scl);
    Wire.setClock(400000L);

    oled.begin(&Adafruit128x64, I2C_ADDRESS, pin_rst);
    oled.setFont(font5x7);
    oled.setScrollMode(SCROLL_MODE_AUTO);
    oled.displayRemap(doRemap);
    
    oled.clear();
    oled.println(deviceID);
}

// Initialise the Wifi and MQTT services - return true if successful
bool kwHeltecWifikit32::initNetwork(const char* wifi_ssid, const char* wifi_pwd, IPAddress mqtt_host)
{
    oled.println("Connecting to wifi");
    oled.println(wifi_ssid);

    mqttClient.setServer(mqtt_host, 1883);
    mqttClient.setCallback(mqttCallback);

    WiFi.mode(WIFI_STA);
    while (WiFi.status() != WL_CONNECTED) 
    {
        int max_tries = 20;
        oled.println("Connecting");
        
        WiFi.disconnect(true);
        // TODO: Fix password case
        WiFi.begin(wifi_ssid);
        while (WiFi.status() != WL_CONNECTED && max_tries-- > 0) 
        {
            oled.print(".");
            delay(1000);
        }
        oled.println();
    };

    mqttReconnect();

}

// Make topic methods
void kwHeltecWifikit32::makeTopic(const char* type, const char* field, const char* sensorType, char* buf)
{
    snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s/%s", TOPIC_ROOT, type, field, sensorType, deviceID);
}

void kwHeltecWifikit32::makeTopic(const char* type, const char* field, char* buf)
{
    snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s", TOPIC_ROOT, type, field, deviceID);
}


// Run - keep MQTT alive and process commands
void kwHeltecWifikit32::run()
{   
    if (!mqttClient.connected()) 
    {
        long now = millis();
        if (now - lastReconnectAttempt > MQTT_RECONNECT_TIME_SECONDS * 1000) 
        {            
            lastReconnectAttempt = now; 
            oled.println("Reconnecting...");
            
            if (mqttReconnect()) 
            {
                lastReconnectAttempt = millis();
            }
        }
        else {} // Wait for timer to expire 
    } 
    else 
    {
        mqttClient.loop();
    }
}


// PRIVATE ///////////////////////////////////////////////////////////////////////

// Populate the device ID
void kwHeltecWifikit32::getMacAddress()
{
    String theAddress = WiFi.macAddress();
    theAddress.replace(":", "");
    strcpy(deviceID, theAddress.c_str());
}

// MQTT connect
boolean kwHeltecWifikit32::mqttReconnect() 
{
    if (mqttClient.connect(deviceID, topicMetaStatus, 2, true, "OFFLINE")) 
    {
        oled.println("MQTT connected");
        mqttClient.publish(topicMetaStatus, "ONLINE");
        // mqttClient.subscribe("inTopic");
        lastReconnectAttempt = millis();
    }
    return mqttClient.connected();
}

// MQTT callback function
void kwHeltecWifikit32::mqttCallback(char* topic, byte* payload, unsigned int length)
{

}

