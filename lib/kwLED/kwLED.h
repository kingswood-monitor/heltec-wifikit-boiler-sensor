#ifndef H_KWLED
#define H_KWLED

#include <Arduino.h>

typedef enum {
    LED_ON = 0,
    LED_OFF,
    LED_TOGGLE,
    LED_FLASH_ONCE,
    LED_FLASH_SLOW,
    LED_FLASH_FAST
} led_id_t;


class kwLED
{
public:
    kwLED(int pin);
    void command(led_id_t command);

protected:
    int m_pin;
    bool m_state = LOW;
};


// kwLED constructor
kwLED::kwLED(int pin)
{
    m_pin = pin;
    pinMode(m_pin, OUTPUT);
    digitalWrite(m_pin, LOW);
}


void kwLED::command(led_id_t command)
{
    switch(command) {
        
        case LED_ON:
            digitalWrite(m_pin, HIGH);
            m_state = HIGH;
            break;
        
        case LED_OFF:
            digitalWrite(m_pin, LOW);
            m_state = LOW;
            break;
        
        case LED_TOGGLE:
            if (m_state == LOW)
            {
                digitalWrite(m_pin, HIGH);
                m_state = HIGH;
            }
            else{
                digitalWrite(m_pin, LOW);
                m_state = LOW;
            }
            break;
        
        case LED_FLASH_ONCE:
            digitalWrite(m_pin, HIGH);
            delay(50);
            digitalWrite(m_pin, LOW);
    }
}


#endif
