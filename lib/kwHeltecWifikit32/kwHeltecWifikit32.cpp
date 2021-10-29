#include <kwHeltecWifikit32.h>

SSD1306AsciiWire oled;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

char buf[10] = {0};

// PUBLIC ///////////////////////////////////////////////////////////////////////

// kwHeltecWifikit32 constructor
kwHeltecWifikit32::kwHeltecWifikit32()
{
    getMacAddress();
    makeTopic("meta", "status", topicMetaStatus);
    pinMode(LED, OUTPUT);
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

    // NOTE - this gives a 4 x 13 usable display
    // oled.setRow(row * oled.fontRows());
    // oled.setCol(col * oled.fontWidth());
    oled.setFont(Callibri15);
    oled.setLetterSpacing(2);
    maxRows = oled.displayHeight() / (oled.fontRows() * 8); // 4
    maxCols = oled.displayWidth() / oled.fontWidth();       // 13

    oled.displayRemap(doRemap);
    oled.clear();
    
    updateSystemStatus(deviceID);
}

// Initialise the Wifi and MQTT services - return true if successful
bool kwHeltecWifikit32::initNetwork(const char* wifi_ssid, const char* wifi_pwd, IPAddress mqtt_host)
{
    updateSystemStatus("Connecting to WiFi");

    mqttClient.setServer(mqtt_host, 1883);
    mqttClient.setCallback(mqttCallback);

    WiFi.mode(WIFI_STA);
    while (WiFi.status() != WL_CONNECTED) 
    {
        int max_tries = 20;
        
        WiFi.disconnect(true);
        // TODO: Fix password case
        WiFi.begin(wifi_ssid);
        while (WiFi.status() != WL_CONNECTED && max_tries-- > 0) 
        {
            delay(1000);
        }
        oled.println();
    };

    updateSystemStatus("Connected to WiFi");

    mqttReconnect();

}

// Make topic methods
void kwHeltecWifikit32::makeTopic(const char* type, const char* field, const char* sensorType, char* buf)
{
    snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s/%s", TOPIC_ROOT, type, field, sensorType, deviceID);
    // if its a command, register it
    if (strcmp(type, "command") == 0) { mqttClient.subscribe(buf); }
}

void kwHeltecWifikit32::makeTopic(const char* type, const char* field, char* buf)
{
    snprintf(buf, MAX_TOPIC_BUFFER_LEN, "%s/%s/%s/%s", TOPIC_ROOT, type, field, deviceID);
}

// Publish data to the specified topic
void kwHeltecWifikit32::publish(char* topic, char* data)
{
    mqttClient.publish(topic, data);
}

void kwHeltecWifikit32::publish(char* topic, uint16_t data)
{
    sprintf(buf, "%d", data);
    publish(topic, buf);
}

void kwHeltecWifikit32::publish(char* topic, float data)
{
    sprintf(buf, "%.1f", data);
    publish(topic, buf);
}

// Display the labeled data at the specified row
void kwHeltecWifikit32::display(const char* data, int row)
{
    oled.setCursor(0, row);
    oled.print(data);
    oled.clearToEOL();
}


// Run - keep MQTT alive and process commands
void kwHeltecWifikit32::run()
{   
    if (!mqttClient.connected()) 
    {   
        digitalWrite(LED, LOW);
        
        long now = millis();
        if (now - lastReconnectAttempt > MQTT_RECONNECT_TIME_SECONDS * 1000) 
        {            
            lastReconnectAttempt = now; 
            updateSystemStatus("Connecting to MQTT");
            
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

// Display system status
void kwHeltecWifikit32::updateSystemStatus(std::string statusMessage)
{
    oled.setRow(3 * oled.fontRows());
    oled.setCol(0);
    oled.print(statusMessage.c_str());
    oled.clearToEOL();
}

// Rconnect MQTT
boolean kwHeltecWifikit32::mqttReconnect() 
{
    updateSystemStatus("Connecting to MQTT");

    if (mqttClient.connect(deviceID, topicMetaStatus, 2, true, "OFFLINE")) 
    {
        updateSystemStatus("ONLINE");
        digitalWrite(LED, HIGH);
        
        mqttClient.publish(topicMetaStatus, "ONLINE");
        lastReconnectAttempt = millis();
    }
    return mqttClient.connected();
}

// MQTT callback function
void kwHeltecWifikit32::mqttCallback(char* topic, byte* payload, unsigned int length)
{
    
}

