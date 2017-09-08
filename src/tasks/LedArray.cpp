#ifndef _LED_ARRAY_
#define _LED_ARRAY_
#include "LedControl.h"
#include "../utils.h"
//Max count max7219 modules is 8 -> 16 digits table
/**
* @description Класс для работы со светодиодными табло, экземпляр класса создается
*              для массива подключенных микросхем max7219, максимум в массиве 8
*              микросхем, каждая из которых может управлять 7-сегметными 4х-циферными
*              светодиодными табло
*/
class LedArray{
  public:
    bool completed[16]; // Массив для того, что бы понять какой номер должен мигать
    bool tick[16]; // Массив для реализации мигания номеров
    uint16_t val[16]; // Массив для хранения номеров клиентов
    uint8_t getId(){
      return _id;
    }
    LedArray(uint8_t id, int DIN_pin, int CLK_pin, int LOAD_pin, int MAX7219_COUNT = 1){
      if(MAX7219_COUNT > 8){
        console.log("WARNING MAX7219_COUNT MUST BE < 9");
      }
      // Инициализация
      for(int i = 0; i < 16; i++) {
        completed[i] = false;
        tick[i] = false;
        val[i] = 0;
      }
      _lc = new LedControl(DIN_pin, CLK_pin, LOAD_pin, MAX7219_COUNT);
      _id = id;

      #ifdef TEST_LED_ARRAY
      for(int device=getDeviceCount() * 2;device--;){
          setDisplay(device,1230 + device,0, false);

      }
      delay(50000);
      #endif
    }
    ~LedArray(){
      clean_all_led(true);
      delete _lc;
    }
    /**
    * @description Функция выключает все сегменты одного цифрового табло
    * @param display Номер цифрового табло
    */
    void hideDisplay(int display) {
        for( int i = 4; i--;){
          _lc->setChar( display/2, i + display%2*(4) , ' ', false);
        }
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
    void setDisplay(int display, uint16_t digit_value, uint8_t dots = 0, bool save = true){
      if(save){
        val[display] = digit_value;
        completed[display] = false;
      }

      //убираем лидирующие нули
      uint8_t digits = 0;
      uint16_t temp_digit_value = digit_value;
      while(temp_digit_value /= 10){
        digits++;
      }
      //т.к светодиодных табло очень много, у нас есть SPI лини для управления ими 16 шт и 8шт
      //Первый парамтр дисплей потом следуют цифры хитрая формула
      for(uint16_t i = 4, shift = 10; (i--) - (3 - digits); shift *= 10) {
        _lc->setDigit(display/2, i+display%2*(4), (digit_value % shift) / (shift / 10) , (dots >> i) & 1 );
      }


    }
    void setTimeOnDisplay(int display, int hours, int minutes, bool dots = true){
      setDisplay(display, hours * 100 + minutes, dots ? 2 : 0, false);
    }
    void clearDislays(){
      for(int i = getDeviceCount();i--;)
        _lc->clearDisplay(i);
    }
    void showTestNumbers(uint16_t prefix = 7600){
      for(int device=getDeviceCount() * 2;device--;){
        setDisplay(device, device + prefix, false, false);
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
