#ifndef H_KWBOILER
#define H_KWBOILER

#include <Arduino.h>

class kwBoiler
{
public:
    kwBoiler(int sensorPin);
    uint16_t readState();
    uint16_t activeSeconds();
    void resetActiveSeconds();
    
    int sensorPin = 0;

private:
    uint32_t activeMillis = 0;
};

#endif