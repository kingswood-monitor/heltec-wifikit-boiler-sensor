#include <kwBoiler.h>
#include <Arduino.h>

bool doRBE = false; 
int state = LOW;
int oldState = LOW;
bool didRise;

uint32_t cumulativeTimeMillis = 0;
uint32_t oldMillis;

// kwBoiler constructor
kwBoiler::kwBoiler(int pin) : sensorPin{ pin }
{
    pinMode(sensorPin, INPUT);
}

// Read boiler state
uint16_t kwBoiler::readState()
{
    state = digitalRead(sensorPin);
    
    didRise = (oldState == LOW) && (state == HIGH);
    if (didRise) { oldMillis = millis(); }

    if (state == HIGH)
    {
        cumulativeTimeMillis += millis() - oldMillis;
        oldMillis = millis();
    }

    oldState = state;
    
    return state;
}

// Get cumulative on time in seconds
uint16_t kwBoiler::cumulativeSeconds()
{
    return cumulativeTimeMillis / 1000;
}