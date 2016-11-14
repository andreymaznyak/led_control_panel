#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ProcessScheduler.h>
#include "LedControl.h"
#include "ClockProcess.cpp"
enum LED_CONTROL_PINS{ DIN_pin = D2, CLK_pin=D1, LOAD_pin=D0 };

// Create my custom Blink Process
class LedArrayWatcherProcess : public Process
{
public:
    // Call the Process constructor
    LedArrayWatcherProcess(Scheduler &manager, ProcPriority pr, unsigned int period,const int MAX7219_COUNT, ClockProcess * clock = NULL)
        :  Process(manager, pr, period)
        {
          _clock = clock;
          _display_count = MAX7219_COUNT;
          _led_count = MAX7219_COUNT * 2;
          lc = new LedControl(DIN_pin, CLK_pin, LOAD_pin, MAX7219_COUNT);
        }
    ~LedArrayWatcherProcess(){
      delete lc;
    }
protected:
    virtual void setup()
    {
      clean_all_led();
      Serial.println("LedArrayWatcherProcess started");
    }

     // Undo setup()
    virtual void cleanup()
    {
      clean_all_led(true);
    }

    // Create our service routine
    virtual void service()
    {
      sendRequest();
    }
private:
  int _display_count;
  int _led_count;
  ClockProcess * _clock;
  HTTPClient http;
  LedControl * lc;
  void clean_all_led(bool shutdown = false){
    for(int k = 0; k < _led_count;k++){
     lc->shutdown(k,shutdown);
     /* Set the brightness to a medium values */
     lc->setIntensity(k,15);
     /* and clear the display */
     lc->clearDisplay(k);
    }
  }
  void sendRequest(){


    http.begin("http://192.168.100.172:1337/orders1c/getOrders1cQueue");
    http.addHeader("cookie",   "magic1cCookie");
    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        StaticJsonBuffer<1024> jsonBuffer;

        // file found at server
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            //Serial.println(payload);
            JsonObject& root = jsonBuffer.parseObject(payload);
            if (!root.success())
            {
              Serial.println("parseObject() failed");
              return;
            }
            int length = root["length"];
            for(int k = _led_count; k--;){
              lc->clearDisplay(k);
            }
            for(int k = 0; k < length; k++){
              int number = root["grouped"][k];
              if(number > 0){
                bool zeros = true;
                for(int i = 0, shift=10000; i < 4;i++,shift /=10){
                  if(!zeros ||  ((number % shift) / (shift / 10))>0){
                    lc->setDigit((k+1)/2, i+(4*((k+1)%2)), (number % shift) / (shift / 10), false);
                    zeros = false;
                  }
                }
              }
              Serial.println(number);

            }
            if(_clock!=NULL){
              //hours
              lc->setDigit(0,0,(_clock->getHours()%100) / 10, false);
              lc->setDigit(0,1,_clock->getHours()%10, true);
              //minutes
              lc->setDigit(0,2,(_clock->getMinutes()%100) / 10, false);
              lc->setDigit(0,3,_clock->getMinutes()%10, false);
            }

        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
};
