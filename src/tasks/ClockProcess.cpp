#ifndef _CLOCK_PROCESS_

#define _CLOCK_PROCESS_

#include <ProcessScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>

#include <WiFiUdp.h>
#include "ClockProcess.cpp"
//#include "ClockProcess.h"

// Create my custom Blink Process
class ClockProcess : public Process
{
public:
    // Call the Process constructor
    ClockProcess(Scheduler &manager, ProcPriority pr, unsigned int period, const char* password = "")
        :  Process(manager, pr, period)
        {
          timeClient = new NTPClient(ntpUDP, "europe.pool.ntp.org", 60*60*10, 60000);
        }
    ~ClockProcess(){
      delete timeClient;
    }
    int getHours(){
      timeClient->update();
      return timeClient->getHours();
    }
    int getMinutes(){
      timeClient->update();
      return timeClient->getMinutes();
    }
protected:
    virtual void setup()
    {
      timeClient->begin();
      Serial.println("ClockProcess started");
    }

     // Undo setup()
    virtual void cleanup()
    {

    }

    // Create our service routine
    virtual void service()
    {
      timeClient->update();
    }
private:
  WiFiUDP ntpUDP;
  NTPClient * timeClient;
};

#endif
