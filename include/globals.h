#ifndef H_GLOBALS
#define H_GLOBALS

#include <kwLED.h>

#define PIN_SENSOR 27

#define COL2 50

const char *g_firmwareVersion = "1.1.0";
char g_deviceID[16] = { 0 };
int g_sampleIntervalSeconds = 2;

kwLED led(LED);

#endif