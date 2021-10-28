#ifndef H_KWBOILER
#define H_KWBOILER

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "heltec.h"

#define PIN_SENSOR 27

bool doRBE = false; 
int boilerState = LOW;
int oldState = LOW;
bool didRise;

uint32_t cumulativeTimeMillis = 0;
uint32_t riseTimeMillis;
uint32_t onTimeMillis;
uint32_t oldMillis;

void readBoiler()
{
    boilerState = digitalRead(PIN_SENSOR);

    didRise = (oldState == LOW) && (boilerState == HIGH);

    if (didRise) { oldMillis = xTaskGetTickCount(); }
    
    if (boilerState == HIGH)
    {
        cumulativeTimeMillis += xTaskGetTickCount() - oldMillis;
        oldMillis = xTaskGetTickCount();
    }

    oldState = boilerState;
}

int getBoilerState()
{
    return boilerState;
}

void cumulativeTimeSeconds(char *buf)
{
    sprintf(buf, "%d", cumulativeTimeMillis / 1000);
}

#endif