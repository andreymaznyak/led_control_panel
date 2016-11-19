#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ProcessScheduler.h>

#include "./tasks/OTAUpdateProcess.cpp"
#include "./tasks/LedArrayWatcherProcess.cpp"
#include "./tasks/ClockProcess.cpp"
#include "./utils.h"

const int MAX7219_COUNT = 12;

Scheduler sched;


//enum LED_CONTROL_PINS1{ DIN_pin = D2, CLK_pin = D1, LOAD_pin = D0 };
//enum LED_CONTROL_PINS2{ DIN_pin1 = D7, LOAD_pin1 = D6, CLK_pin1 = D5 };
LedArray * lc[2];

void setup() {
  //Serial.begin(115200);
  console.begin();
  const char* ssid = "Garden_Torgoviy_ZAL_2";//"Garden_Torgoviy_ZAL_2";//"Andrewhous";
  const char* password = "blackcar";//"blackcar";//"1234567891111";

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //Serial.println("Connection Failed! Rebooting...");
    console.log("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  if(MAX7219_COUNT < 9)
    lc[0] = new LedArray(1,DIN_pin, CLK_pin, LOAD_pin, MAX7219_COUNT);
  else{
    lc[0] = new LedArray(1,DIN_pin, CLK_pin, LOAD_pin, 8);
    lc[1] = new LedArray(1,D7,D5,D6, MAX7219_COUNT - 8);
  }

  static OTAUpdateProcess otaUpdate(sched,HIGH_PRIORITY,250,"0819");
  static ClockProcess clock(sched, HIGH_PRIORITY,1000, lc);
  static LedArrayWatcherProcess ledArrWather(sched, MEDIUM_PRIORITY, 3000, MAX7219_COUNT, &clock, lc);

  console.log(WiFi.localIP());
  otaUpdate.add(true);
  ledArrWather.add(true);
  clock.add(true);
}
void loop() {
  sched.run();
}
