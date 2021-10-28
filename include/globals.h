#ifndef H_GLOBALS
#define H_GLOBALS

#include <kwLED.h>

#define COL2 50

TimerHandle_t readBoilerTimer;

const char *g_firmwareVersion = "1.1.0";
char g_deviceID[16] = { 0 };
int g_sampleIntervalSeconds = 1;

kwLED led(LED);

#endif