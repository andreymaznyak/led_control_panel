#ifndef _LED_ARRAY_
#define _LED_ARRAY_
#include "LedControl.h"
#include "../utils.h"
//Max count max7219 modules is 8 -> 16 digits table
class LedArray{
  public:
    uint8_t getId(){
      return _id;
    }
    LedArray(uint8_t id, int DIN_pin, int CLK_pin, int LOAD_pin, int MAX7219_COUNT = 1){
      if(MAX7219_COUNT > 8){
        console.log("WARNING MAX7219_COUNT MUST BE < 9");
      }
      _lc = new LedControl(DIN_pin, CLK_pin, LOAD_pin, MAX7219_COUNT);
      _id = id;
      for(int addr = MAX7219_COUNT; addr--;){
        _lc->shutdown(addr,false);
        /* Set the brightness to a medium values */
        _lc->setIntensity(addr,15);
        /* and clear the display */
        _lc->clearDisplay(addr);
        //console.log("clear ");
        //console.log(addr);
      }
      for(int device=getDeviceCount() * 2;device--;){
          setDisplay(device,1230 + device);
      }
      delay(1000);
      for(int addr = MAX7219_COUNT; addr--;){
        _lc->shutdown(addr,false);
        /* Set the brightness to a medium values */
        _lc->setIntensity(addr,15);





        /* and clear the display */
        _lc->clearDisplay(addr);
        //console.log("clear ");
        //console.log(addr);
      }
    }
    ~LedArray(){
      clean_all_led(true);
      delete _lc;
    }
    void clearDisplay(int addr){
      _lc->clearDisplay(addr);
    }
    void setDigit(int chip_number, int digit_number, uint8_t digit_value, bool light_dot = false){
      _lc->setDigit(chip_number, digit_number, digit_value, light_dot);
    }
    void setChar(int chip_number, int digit_number, char digit_value, bool light_dot = false){
      _lc->setChar(chip_number, digit_number, digit_value, light_dot);
    }
    void setDisplay(int display, uint16_t digit_value, uint8_t dots = 0){
      //убираем лидирующие нули
      uint8_t digits = 0;
      uint16_t temp_digit_value = digit_value;
      while(temp_digit_value /= 10){
        digits++;
      }
      for(uint16_t i = 4, shift = 10; (i--) - (3 - digits); shift *= 10)
        _lc->setDigit(display/2, i+display%2*(4), (digit_value % shift) / (shift / 10) , (dots >> i) & 1 );
    }
    void setTimeOnDisplay(int display, int hours, int minutes, bool dots = true){
      setDisplay(display, hours * 100 + minutes, dots ? 2 : 0);
    }
    void showTestNumbers(uint16_t prefix = 7600){
      for(int device=getDeviceCount() * 2;device--;){
        setDisplay(device, device + prefix, false);
      }
    }
  protected:
  private:
    LedControl * _lc;
    uint8_t _id;
    int _display_count;
    int getDeviceCount(){
      return _lc->getDeviceCount();
    };
    void clean_all_led(bool shutdown = false){
      for(int k = getDeviceCount(); k--;){
       _lc->shutdown(k,shutdown);
       /* Set the brightness to a medium values */
       _lc->setIntensity(k,15);
       /* and clear the display */
       _lc->clearDisplay(k);
      }
    }
};

#endif
