                                                                              #ifndef _CLOCK_PROCESS_

#define _CLOCK_PROCESS_

#include <ProcessScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>

#include <WiFiUdp.h>
#include "ClockProcess.cpp"
#include "LedArray.cpp"
#include "WebSocketProcess.cpp"
/**
* @description Класс для показа времени на верхнем светодиодном табло, реализован
*              с помощью ntp сервиса europe.pool.ntp.org
*/
class ClockProcess : public Process
{
public:
    // Call the Process constructor
    ClockProcess(Scheduler &manager, ProcPriority pr, unsigned int period, LedArray * lc[2] = NULL, WebSocketProcess * ws = NULL)
        :  Process(manager, pr, period)
        {
          timeClient = new NTPClient(ntpUDP, "europe.pool.ntp.org", 60*60*10, 60000);
          if(lc != NULL)
            for(uint8_t i = 2; i--;){
              _lc[i] = lc[i];
            }
          _ws = ws;
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
    int getSeconds(){
      timeClient->update();
      return timeClient->getSeconds();
    }

protected:
    virtual void setup()
    {
      timeClient->begin();

      #ifdef DEBUG_SERIAL
      Serial.println("ClockProcess started");
      #endif
    }

     // Undo setup()
    virtual void cleanup()
    {

    }

    // Create our service routine
    virtual void service()
    {
      if(_lc[0] != NULL){
        _clock_seconds_dot_state = !_clock_seconds_dot_state;
        _lc[0]->setTimeOnDisplay(0, getHours(), getMinutes(), _clock_seconds_dot_state);
      }

      timeClient->update();
    }
private:
  WiFiUDP ntpUDP;
  int _heapSize;
  WebSocketProcess * _ws = NULL;
  NTPClient * timeClient;
  LedArray * _lc[2];
  bool _clock_seconds_dot_state = false;
};

#endif
