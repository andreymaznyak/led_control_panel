                                                                              #ifndef _CLOCK_PROCESS_

#define _CLOCK_PROCESS_

#include <ProcessScheduler.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>

#include <WiFiUdp.h>
#include "ClockProcess.cpp"
#include "LedArray.cpp"
#include "WebSocketProcess.cpp"
// Create my custom Blink Process
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

      // int current_heap = ESP.getFreeHeap();
      // if(current_heap != _heapSize){
      //   char message[64];
      //   sprintf(message,"{\"action\":\"Heap\",\"message\":\"Heap size %d -> %d\"}\n", _heapSize, current_heap);
      //   Serial.println(message);
      //   _heapSize = current_heap;
      //   if(_ws != NULL){
      //     _ws->send((char*)&message);
      //   }
      // }
      if(_ws != NULL){
        //_ws->message("ClockProcess started", CLIENT_MESSAGE);
      }
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
        //Serial.println("time update");
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
