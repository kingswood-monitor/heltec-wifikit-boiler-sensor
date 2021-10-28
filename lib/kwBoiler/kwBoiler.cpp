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
int kwBoiler::readState()
{
    state = digitalRead(sensorPin);
    return state;
}