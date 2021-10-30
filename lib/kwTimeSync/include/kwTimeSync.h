#ifndef H_KWTIMESYNC
#define H_KWTIMESYNC

#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiUdp.h>

class kwTimeSync
{
public:
    kwTimeSync();

};

time_t getNtpTime();
void sendNTPpacket(IPAddress &address);


#endif