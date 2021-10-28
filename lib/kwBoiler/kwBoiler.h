#ifndef H_KWBOILER
#define H_KWBOILER

class kwBoiler
{
public:
    kwBoiler(int sensorPin);
    int readState();
    int cumulativeSeconds();
    
    int sensorPin = 0;

private:
};

// bool doRBE = false; 
// int boilerState = LOW;
// int oldState = LOW;
// bool didRise;

// uint32_t cumulativeTimeMillis = 0;
// uint32_t riseTimeMillis;
// uint32_t onTimeMillis;
// uint32_t oldMillis;

// void readBoiler()
// {
//     boilerState = digitalRead(PIN_SENSOR);

//     didRise = (oldState == LOW) && (boilerState == HIGH);

//     if (didRise) { oldMillis = millis(); }
    
//     if (boilerState == HIGH)
//     {
//         cumulativeTimeMillis += millis() - oldMillis;
//         oldMillis = millis();
//     }

//     oldState = boilerState;
// }

// int getBoilerState()
// {
//     return boilerState;
// }

// void cumulativeTimeSeconds(char *buf)
// {
//     sprintf(buf, "%d", cumulativeTimeMillis / 1000);
// }

#endif