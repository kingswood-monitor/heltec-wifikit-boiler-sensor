#ifndef H_GLOBALS
#define H_GLOBALS

#include <kwLED.h>

const char *g_firmwareVersion = "1.1.0";
char g_deviceID[16] = { 0 };
int g_sampleIntervalSeconds = 1;

TimerHandle_t readBoilerTimer;

SSD1306AsciiWire oled;
kwLED led(LED);

// OLED library
#define I2C_ADDRESS 0x3C
#define RST_PIN 16
#define PIN_SDA 4
#define PIN_SCL 15


#endif