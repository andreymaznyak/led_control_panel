/* ------------------------------ PCF8574/a WIRING ------------------------------------
Basic Address:  00100 A2 A1 A0 (from 0x20 to 0x27)
Basic Address:  00111 A2 A1 A0 (from 0x38 to 0x3F)A version
A2,A1,A0 tied to ground = 0x20 (pcf8574) or 0x38 (pcf8574a)
				__ __PCF
		A0    [|  U  |] ++++
		A1    [|     |] sda <->
	  A2    [|     |] scl <->
		IO-0  [|     |] INT ->
		IO-1  [|     |] IO-7
		IO-2  [|     |] IO-6
		IO-3  [|     |] IO-5
		GND   [|_____|] IO-4

SN72HC595
  OUT 1  [|  U  |] ++++
  OUT 2  [|     |] OUT 0
  OUT 3  [|     |] DATA <->
  OUT 4  [|     |] ENABLE -> GND
  OUT 5  [|     |] LATCH <->
  OUT 6  [|     |] CLOCK <->
  OUT 7  [|     |] RESET -> GND
   GND   [|_____|] OVERFLOW
*/
#ifndef _BUTTONS_ARRAY_WATCHER_PROCESS_
#define _BUTTONS_ARRAY_WATCHER_PROCESS_

#include <ProcessScheduler.h>
#include <Arduino.h>
#include <Wire.h>
#include "WebSocketProcess.cpp"
#include "TcpSocketProcess.cpp"
#include "LedArray.cpp"
#include "config.h"
//74HC595 and PCF8574ap
//enum defined_pins { SDA_PIN = D9, SCL_PIN = D10,CLOCK_PIN = D3,LATCH_PIN = D4,DATA_PIN = D8  };
#define LED_ARRAY_LENGH 24

// Create my custom Blink Process
class ButtonsArrayWatcherProcess : public Process
{
public:
  // Call the Process constructor
  ButtonsArrayWatcherProcess(Scheduler &manager,
                             ProcPriority pr,
                             unsigned int period,
                             uint8_t _SDA_PIN = D9,
                             uint8_t _SCL_PIN = D10,
                             uint8_t _CLOCK_PIN = D3,
                             uint8_t _LATCH_PIN = D4,
                             uint8_t _DATA_PIN = D8,
                             LedArray *lc[2] = NULL,
                             WebSocketProcess *ws = NULL,
                             TcpSocketProcess *tcp = NULL)
      : Process(manager, pr, period)
  {
    postSetup(0x38);
    _ws = ws;
    _tcp = tcp;
    SDA_PIN = _SDA_PIN;
    SCL_PIN = _SCL_PIN;
    CLOCK_PIN = _CLOCK_PIN;
    LATCH_PIN = _LATCH_PIN;
    DATA_PIN = _DATA_PIN;
    _gpioDirection = 0;
    _gpioInputs = 0;
    _gpioState = 0;
    if (lc != NULL)
      for (uint8_t i = 2; i--;)
      {
        _lc[i] = lc[i];
      }
  }
  //                          1      2             4      5      6             8      9
  // pins sequence col : row 0 0 -> 0 1 -> ... -> 0 3 -> 1 3 -> 1 2 -> ... -> 1 0 -> 2 0 -> 2 1 -> ... ... -> 5 1 -> 5 0
  uint8_t getNubmerByCoordinates(uint8_t col, uint8_t row)
  {
    return (col % 2 ? (col * 4 + (4 - row)) : (col * 4 + 1 + row)) > 23 ? 23 : (col % 2 ? (col * 4 + (4 - row)) : (col * 4 + 1 + row));
  }
  //return column and row to second and third param
  void getCoordinatesByNubmer(uint8_t number, uint8_t &col, uint8_t &row)
  {
    col = (number - 1) / 4;
    row = col % 2 ? 3 - ((number - 1) % 4) : ((number - 1) % 4);
  }

protected:
  virtual void setup()
  {
    delay(100);

    begin(SDA, SCL);
    gpioPinMode(INPUT); //_PULLUP
    gpioPort(0);
#ifdef DEBUG_SERIAL
    Serial.println("ButtonsArrayWatcherProcess starting");
#endif

    readGpio();
#ifdef DEBUG_SERIAL
    for (uint8_t t = 8; t--;)
      Serial.print(gpioDigitalRead(t));

    readGpio();
    for (uint8_t t = 8; t--;)
      Serial.printf("%d", (_gpioState >> t) & 1);
#endif
    readGpio();
  }

  // Undo setup()
  virtual void cleanup()
  {
  }

  // Create our service routine
  virtual void service()
  {
    //Serial.begin(1);
    //delay(10);
    //Serial.end();
    //delay(10);
    //устанавливаем режим OUTPUT
    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);

    char str[64];
    bool print = false;
    for (uint8_t row = 8; row--;)
    {
      setData(1 << row);
      // uint8_t old_state = _gpioState;
      // readGpioPort();
      // if(_gpioState != old_state){
      //   for(uint8_t t = 4; t--;){
      //     Serial.printf("col %d ", i);
      //     Serial.printf("row %d \n", t);
      //     //Serial.printf("%d", (_gpioState >> t) & 1 );
      //   }
      //   Serial.printf("\n");
      // }
      readGpioPort();
      //Serial.printf("row %d ", row);
      for (uint8_t col = 4; col--;)
      {
        uint8_t index = getNubmerByCoordinates(row - 1, col) - 1;
        if (gpioDigitalReadFast(col))
        {
          if ((_buttons_status[index] & 1) == 0)
          {
            //TODO Событие нажатия кнопки
            _buttons_status[index] = _buttons_status[index] | 1;
            _buttons_status[index] += 2;
            print = true;

            uint8_t arr_index = index < 16 ? 0 : 1,
                    arr_number = (index < 16 ? index : index - 16) + 1;

            _lc[0]->setChar(0, 0, '5');
            _lc[0]->setChar(0, 1, 'E');
            _lc[0]->setChar(0, 2, 'n');
            _lc[0]->setChar(0, 3, 'd');
#if BUTTONS_DEBUG > 0
            _lc[arr_index]->setDisplay(arr_number, _buttons_status[index] >> 1);
#else
            _lc[arr_index]->setDisplay(arr_number, 8888);
            _tcp->sendButtonPress(index);
            //if (_ws != NULL)
            //{
            // sprintf(str, "{\"number\":%d,\"row\":%d,\"col\":%d,\"count\":%d}", index, row, col, _buttons_status[index] >> 1);
            // _ws->json((char *)&str, PRESS_BUTTON);
            _lc[arr_index]->setDisplay(arr_number, 0);
            //_lc[arr_index]->hideDisplay(arr_number + 1);
            _lc[arr_index]->val[arr_number] = 0;
            _lc[arr_index]->completed[arr_number] = 0;

            // Для отладки добавить ESP.restart();
            // ESP.restart();
            //}
#endif
            //Serial.printf("number %d (row %d, col %d) count %d on ...", index, row, col,  _buttons_status[index] >> 1);
          }
        }
        else if (_buttons_status[index] & 1)
        {
          _buttons_status[index] = _buttons_status[index] & ~1;
          //Serial.printf(" --> off \n");
          print = true;
          sprintf(str, " --> off \n");
        }
      }
    }
    Serial.begin(115200);
    delay(10);
#ifdef DEBUG_SERIAL
    Serial.printf("%s\n", str);
#endif
  }

private:
  uint8_t _buttons_status[LED_ARRAY_LENGH];
  uint8_t _adrs;
  uint8_t _gpioDirection;
  uint8_t _gpioState;
  uint8_t _gpioInputs;
  LedArray *_lc[2];
  WebSocketProcess *_ws;
  TcpSocketProcess *_tcp;
  uint8_t SDA_PIN, SCL_PIN, CLOCK_PIN, LATCH_PIN, DATA_PIN;
  bool _error;
  void updateGpio()
  {
    if (!_error)
    {
      uint8_t data = (_gpioInputs & ~_gpioDirection) | _gpioState;
      Wire.beginTransmission(_adrs);
      Wire.write(data);
      Wire.endTransmission();
    }
  }
  void readGpio()
  {
    if (!_error)
    {
      Wire.requestFrom((uint8_t)_adrs, (uint8_t)1);
      _gpioState = Wire.read();
    }
  }
  //used with other libraries only
  void postSetup(const uint8_t adrs)
  {
    if (adrs >= 0x38 && adrs <= 0x3F)
    { // 0x38...0x3F for A variant!
      _adrs = adrs;
      _error = false;
    }
    else
    {
      _error = true;
    }
  }

  //protocolInitOverride=true	will not init the SPI
  void begin(uint8_t SDA = D2, uint8_t SCL = D1)
  {

    Wire.begin(D2, D1);
#if ARDUINO >= 157
    Wire.setClock(400000UL); // Set I2C frequency to 400kHz
#else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; // Set I2C frequency to 400kHz
#endif
    delay(100);
    readGpio();
  }
  //OUTPUT=all out,INPUT=all in,INPUT_PULLUP in,0xxxx=you choose
  void gpioPinMode(uint8_t mode)
  {
    if (mode == INPUT)
    {
      _gpioDirection = 0xFF;
    }
    else if (mode == INPUT_PULLUP)
    {
      _gpioDirection = 0x00;
      _gpioState = 0xFF;
    }
    else if (mode == OUTPUT)
    {
      _gpioDirection = 0x00;
      _gpioState = 0x00;
    }
    else
    {
      _gpioDirection = mode;
    }
    updateGpio();
  }
  //set a unique pin as INPUT,INPUT_PULLUP,OUTPUT
  void gpioPinMode(uint8_t pin, uint8_t mode)
  {
    if (pin < 8)
    { //0...7
      switch (mode)
      {
      case INPUT:
        _gpioDirection &= ~(1 << pin);
        _gpioState &= ~(1 << pin);
        break;
      case INPUT_PULLUP:
        _gpioDirection &= ~(1 << pin);
        _gpioState |= (1 << pin);
        break;
      case OUTPUT:
        _gpioDirection |= (1 << pin);
        _gpioState &= ~(1 << pin);
        break;
      default:
        break;
      }
      updateGpio();
    }
  }
  //HIGH=all Hi, LOW=all Low,0xxxx=you choose witch low or hi
  void gpioPort(uint8_t value)
  {
    if (value == HIGH)
    {
      _gpioState = 0xFF;
    }
    else if (value == LOW)
    {
      _gpioState = 0x00;
    }
    else
    {
      _gpioState = value;
    }
    updateGpio();
  }
  //read the state of the pins (all)
  uint8_t readGpioPort()
  {
    readGpio();
    return _gpioState;
  }
  //experimental! Works only when pins are outs!
  uint8_t readGpioPortFast()
  {
    return _gpioState;
  }
  //write data to one pin
  void gpioDigitalWriteFast(uint8_t pin, bool value)
  {
    if (pin < 8)
    { //0...7
      value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
    }
  }
  void gpioDigitalWrite(uint8_t pin, bool value)
  {
    if (pin < 8)
    { //0...7
      value == HIGH ? _gpioState |= (1 << pin) : _gpioState &= ~(1 << pin);
      updateGpio();
    }
  }
  //read data from one pin
  uint8_t gpioDigitalRead(uint8_t pin)
  {
    readGpio();
    return (_gpioState & (1 << pin)) ? HIGH : LOW;
  }
  //experimental! Works only when pin are outs!
  uint8_t gpioDigitalReadFast(uint8_t pin)
  {
    int temp = 0;
    if (pin < 8)
      temp = bitRead(_gpioState, pin); //0...7
    return temp;
  }

  void gpioPinToggle(uint8_t pin)
  {
    _gpioState ^= (1 << pin);
    updateGpio();
  }

  void gpioPortUpdate()
  {
    if (!_error)
    {
      //uint8_t data = (_gpioInputs & ~_gpioDirection) | _gpioState;
      Wire.beginTransmission(_adrs);
      Wire.write(_gpioState);
      Wire.endTransmission();
    }
  }

  void setData(uint8_t data)
  {

    digitalWrite(LATCH_PIN, LOW);
    // передаем последовательно на dataPin
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);

    //"защелкиваем" регистр, тем самым устанавливая значения на выходах
    digitalWrite(LATCH_PIN, HIGH);
  }
};
#endif
