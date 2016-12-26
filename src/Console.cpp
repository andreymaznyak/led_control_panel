#ifndef _CONSOLE_
#define _CONSOLE_
#include <Arduino.h>
//#include "./tasks/WebSocketProcess.cpp"
//#include "config.h"
template <class Type> void log(Type message);

class Console{
  public:
    unsigned long getBaudRate(){
      return _serial_baud_rate;
    }
    Console(unsigned long serial_baud_rate){
      //Serial.begin(serial_baud_rate);
    }
    ~Console(){

    }
    // void begin(WebSocketProcess * ws){
    //   _ws = ws;
    // }
    uint8_t printf(const char *format, ...){
        va_list arg;
        va_start(arg, format);
        char temp[128];
        char* buffer = temp;
        size_t len = vsnprintf(temp, sizeof(temp), format, arg);
        va_end(arg);
        if (len > sizeof(temp) - 1) {
            buffer = new char[len + 1];
            if (!buffer) {
                return 0;
            }
            va_start(arg, format);
            ets_vsnprintf(buffer, len + 1, format, arg);
            va_end(arg);
        }
        //_ws->send(buffer, CLIENT_MESSAGE);
        if (buffer != temp) {
            delete[] buffer;
        }
        return len;
    }
    template <class Type>
    void println(Type message){
      // _ws->send(message, CLIENT_MESSAGE);
    }
    template <class Type>
    void print(Type message){
      // _ws->send(message, CLIENT_MESSAGE);
    }
    template <class T> void log(T message){
      //_ws->send(message);
    }

  private:
    unsigned long _serial_baud_rate;
    // WebSocketProcess * _ws;
};

#endif
