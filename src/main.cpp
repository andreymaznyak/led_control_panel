#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ProcessScheduler.h>

#include "./tasks/OTAUpdateProcess.cpp"
#include "./tasks/LedArrayWatcherProcess.cpp"
#include "./tasks/ClockProcess.cpp"
const int MAX7219_COUNT = 16;

Scheduler sched;
OTAUpdateProcess otaUpdate(sched,HIGH_PRIORITY,250,"0819");
ClockProcess clock(sched, HIGH_PRIORITY,1000);
LedArrayWatcherProcess ledArrWather(sched, HIGH_PRIORITY, 3000, MAX7219_COUNT, &clock);

void setup() {
  Serial.begin(115200);
  const char* ssid = "Garden_Torgoviy_ZAL_2";//"Andrewhous";
  const char* password = "blackcar";//"1234567891111";

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println(WiFi.localIP());
  otaUpdate.add(true);
  ledArrWather.add(true);
  clock.add(true);
}
void loop() {
  sched.run();
}
