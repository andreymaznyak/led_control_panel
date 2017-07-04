#ifndef CONFIG
#define CONFIG
#include <Arduino.h>

#define BTN_WATCHER_ENABLE

#define DEBUG_SERIAL
#define BUTTONS_DEBUG 0
//#define DEVICE_ID 4
#define MAX7219_COUNT 12
//Этот параметр должен быть уникальный для каждой стойки, ssid модуля
// /device/0 и /device/1 - самовывоз /device/2 и /device/3 - доставка
#define DEVICE_ID "/device/3" // 0 -> 192.168.100.143 ; 1 -> 135 ; 2 -> 139; 3 -> 146

#define SSID "garden"
//"Garden_Torgoviy_ZAL_2";//"Andrewhous";"garden"
//extern const char* password;
#define PASSWORD "33333333"
//"blackcar";//"1234567891111";"33333333"


/* PIN CONFIG
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
enum CONTROL_PINS{ LEDTABLE1_DIN_pin = D0,//D0
                   LEDTABLE1_LOAD_pin = D3,//D1
                   LEDTABLE1_CLK_pin = D4,//D2

                   LEDTABLE2_DIN_pin = D5,//D7
                   LEDTABLE2_CLK_pin = D7,//D5
                   LEDTABLE2_LOAD_pin = D6,//

                   PCF8574AP_SDA_PIN = D2,// WORK ONLY D2
                   PCF8574APSCL_PIN = D1,// WORK ONLY D1
                   SN74HC595_CLOCK_PIN = D8,//SH D3
                   SN74HC595_LATCH_PIN = D9,//ST D4
                   SN74HC595_DATA_PIN = D10 };


#endif
