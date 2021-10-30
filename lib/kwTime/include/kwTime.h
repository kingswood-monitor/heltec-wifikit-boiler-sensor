#ifndef H_KWTIMESYNC
#define H_KWTIMESYNC

#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class kwTime
{
public:
    kwTime();
    void init();
    void displayTime();
};

time_t getNtpTime();
void sendNTPpacket(IPAddress &address);
void digitalClockDisplay();
void printDigits(int digits);

#endif