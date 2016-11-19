#ifndef _CONSOLE_
#define _CONSOLE_
#include <Arduino.h>

template <class Type> void log(Type message);

class Console{
  public:
    unsigned long getBaudRate(){
      return _serial_baud_rate;
    }
    Console(unsigned long serial_baud_rate){
      Serial.begin(serial_baud_rate);
    }
    ~Console(){

    }
    void begin(){

    }
    template <class Type>
    void log(Type message){
      Serial.println(message);
    }
  private:
    unsigned long _serial_baud_rate;
};

#endif
