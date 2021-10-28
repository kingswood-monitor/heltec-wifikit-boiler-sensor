#ifndef H_KWBOILER
#define H_KWBOILER

#include <Arduino.h>

class kwBoiler
{
public:
    kwBoiler(int sensorPin);
    uint16_t readState();
    uint16_t cumulativeSeconds();
    
    int sensorPin = 0;

private:
    uint32_t cumulativeTimeMillis = 0;
};

#endif