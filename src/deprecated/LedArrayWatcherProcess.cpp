// #include <ESP8266HTTPClient.h>
// #include <ArduinoJson.h>
// #include <ProcessScheduler.h>
// #include "ClockProcess.cpp"
// #include "LedArray.cpp"
// #include "../utils.h"
//
// //enum LED_CONTROL_PINS{ DIN_pin = D7, CLK_pin=D5, LOAD_pin=D6 };
//
// // Create my custom Blink Process
// class LedArrayWatcherProcess : public Process
// {
// public:
//     // Call the Process constructor
//     LedArrayWatcherProcess(Scheduler &manager, ProcPriority pr, unsigned int period, const int MAX7219_COUNT, ClockProcess * clock = NULL, LedArray * lc[2] = NULL)
//         :  Process(manager, pr, period)
//         {
//           _clock = clock;
//           _display_count = MAX7219_COUNT;
//           _led_count = MAX7219_COUNT * 2;
//           _arr_count = 1 + (MAX7219_COUNT > 8);
//           if(lc != NULL)
//             for(uint8_t i = 2; i--;){
//               _lc[i] = lc[i];
//             }
//
//         }
//     ~LedArrayWatcherProcess(){
//
//     }
//     bool send_req = true;
// protected:
//     virtual void setup()
//     {
//       //clean_all_led();
//       #ifdef DEBUG_SERIAL
//       Serial.println("LedArrayWatcherProcess started");
//       #endif
//     }
//
//      // Undo setup()
//     virtual void cleanup()
//     {
//       //clean_all_led(true);
//     }
//
//     // Create our service routine
//     virtual void service()
//     {
//
//       sendRequest();
//     }
// private:
//   int _display_count;
//   int _led_count;
//   uint8_t _arr_count;
//   ClockProcess * _clock;
//   HTTPClient http;
//   LedArray * _lc[2];
//
//   void sendRequest(){
//
//     http.begin("http://192.168.100.172:1337/orders1c/getOrders1cQueue?row=2");
//     http.addHeader("cookie",   "magic1cCookie");
//     int httpCode = http.GET();
//
//     // httpCode will be negative on error
//     if(httpCode > 0) {
//         // HTTP header has been send and Server response header has been handled
//         //Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//
//         StaticJsonBuffer<1024> jsonBuffer;
//
//         // file found at server
//         if(httpCode == HTTP_CODE_OK) {
//             String payload = http.getString();
//             //Serial.println(payload);
//             JsonObject& root = jsonBuffer.parseObject(payload);
//             if (!root.success())
//             {
//               #ifdef DEBUG_SERIAL
//               Serial.println("parseObject() failed");
//               #endif
//               return;
//             }
//             int length = root["length"];
//             // for(int k = _led_count; k--;){
//             //   lc[0]->clearDisplay(k);
//             // }
//             for(int k = 0; k < length; k++){
//               int number = root["grouped"][k];
//               int position = root["grouped_position"][k];
//               if(number > 0){
//                 bool zeros = true;
//                 _lc[0]->setDisplay(position,number);
//               }
//               Serial.println(number);
//
//             }
//
//
//         }
//     } else {
//         #ifdef DEBUG_SERIAL
//         Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//         #endif
//     }
//     http.end();
//   }
// };
