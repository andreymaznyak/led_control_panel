#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <GDBStub.h>
#include <ProcessScheduler.h>

#include "./tasks/WebSocketProcess.cpp"

#include "config.h"

//#include "./tasks/SocketIoClient.cpp"
#include "./tasks/OTAUpdateProcess.cpp"
//#include "./tasks/LedArrayWatcherProcess.cpp"
#include "./tasks/ClockProcess.cpp"
#include "./utils.h"
#include "./tasks/ButtonsArrayWatcherProcess.cpp"





Scheduler sched;


LedArray * lc[2];
WebSocketProcess * p_ws = NULL;
//static SocketIoClient * ws = NULL;
void setup() {

  Serial.begin(115200);


  WiFi.mode(WIFI_AP);//WIFI_STA);AP
  WiFi.begin(ssid, password);


  delay(5000);
  Serial.println("12342351235\n\n\n ------");
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    #ifdef DEBUG_SERIAL
    Serial.println("Connection Failed! Rebooting...");
    #endif
    delay(5000);
    ESP.restart();
  }
  Serial.println(WiFi.localIP());

  Serial.println("\n\n\n <------>2<----->");
  if(MAX7219_COUNT < 9)
    lc[0] = new LedArray(1,LEDTABLE1_DIN_pin, LEDTABLE1_CLK_pin, LEDTABLE1_LOAD_pin, MAX7219_COUNT);
  else{
    Serial.println("\n\n\n <------>3<----->");
    lc[0] = new LedArray(1,LEDTABLE1_DIN_pin, LEDTABLE1_CLK_pin, LEDTABLE1_LOAD_pin, 8);
    lc[1] = new LedArray(1,LEDTABLE2_DIN_pin,LEDTABLE2_CLK_pin,LEDTABLE2_LOAD_pin, MAX7219_COUNT - 8);
  }
  Serial.println("#@&)#%&)%&");
  //static SocketIoClient socket(sched, LOW_PRIORITY, 3000, server_ip, server_port);
  static WebSocketProcess ws(sched, LOW_PRIORITY, 500, lc);
  Serial.println(1);
  p_ws = &ws;
  static OTAUpdateProcess otaUpdate(sched, LOW_PRIORITY,250,"0819");
  Serial.println(2);
  static ClockProcess clock(sched, MEDIUM_PRIORITY,1000, lc, p_ws);
  Serial.println(3);
  //static LedArrayWatcherProcess ledArrWather(sched, LOW_PRIORITY, 10000, MAX7219_COUNT, &clock, lc);
  #ifdef BTN_WATCHER_ENABLE
  static ButtonsArrayWatcherProcess btnWather(sched, HIGH_PRIORITY, 100, PCF8574AP_SDA_PIN, PCF8574APSCL_PIN, SN74HC595_CLOCK_PIN, SN74HC595_LATCH_PIN, SN74HC595_DATA_PIN, lc, p_ws);
  #endif

  //console.begin(p_ws);
  //ws->add(true);

  //socket.add(true);
//  Serial.println((int)&socket);
  otaUpdate.add(true);
  //ledArrWather.add(true);
  clock.add(true);
  ws.add(true);
  Serial.println(4);
  char ipno2[64];
  IPAddress ipno = WiFi.localIP();
  sprintf(ipno2, "{\"ip\":\"%d.%d.%d.%d\"}", ipno[0], ipno[1], ipno[2], ipno[3]);
  #ifdef DEBUG_SERIAL
  for(int i = 0; i < 64; i++){
    if(ipno2[i] == '\0') {Serial.println("out");break;}
    Serial.printf("%d ",ipno2[i]);
  }
  Serial.println(ipno2);
  #endif


  p_ws->json((char*)(&ipno2), CONNECT);
  Serial.println(5);
  //console.log(WiFi.localIP());
  #ifdef DEBUG_SERIAL
  Serial.println("wait for rx-tx support");
  #endif
  // for(uint8_t w= 6; w--;){
  //   lc[0]->setDisplay(0,w);
  //   Serial.println(w);
  //   delay(1000);
  // }
  //ws->sendMessage("Hello");
  //Serial.println(socket.sendMessage("Hello"))
  #ifdef BTN_WATCHER_ENABLE
  btnWather.add(true);
  #endif

}
void loop() {
  // lc[1]->setDisplay(8,1234);
  // lc[1]->setDisplay(7,1234);
  // lc[1]->setDisplay(6,1234);
  // lc[1]->setDisplay(5,1234);
  // lc[1]->setDisplay(4,1234);
  sched.run();
}
