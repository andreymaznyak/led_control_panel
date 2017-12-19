#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <GDBStub.h>
#include <ProcessScheduler.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>

#include "./tasks/WebSocketProcess.cpp"
#include "./tasks/TcpSocketProcess.cpp"
#include "config.h"

//#include "./tasks/SocketIoClient.cpp"
#include "./tasks/OTAUpdateProcess.cpp"
//#include "./tasks/LedArrayWatcherProcess.cpp"
#include "./tasks/ClockProcess.cpp"
#include "./utils.h"
#include "./tasks/ButtonsArrayWatcherProcess.cpp"

#include "./tasks/CompletedOrdersWatcherProcess.cpp"

Scheduler sched;

LedArray *lc[2] = {NULL,
                   NULL};

// lc[0] = NULL;
// lc[1] = NULL;

WebSocketProcess *p_ws = NULL;
TcpSocketProcess *p_tcp = NULL;

void setupWiFiAP()
{
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = DEVICE_ID_STR;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, "andrewhous");
}

void loadLedArrayFromEEPROM(LedArray *_lc[2])
{
  _lc[0]->setChar(0, 0, 'L');
  _lc[0]->setChar(0, 1, 'd');
  _lc[0]->setChar(0, 2, '0');
  _lc[0]->setChar(0, 3, '1');
  uint8_t buff[48]; // Буффер для данных стоек
  const int startAddress = START_ADDRESS;
  // for (uint8_t i = 0; i < 48; i++)
  // {
  //   EEPROM.write(startAddress + i, 0);
  // }
  // EEPROM.commit();

  for (uint8_t i = 0; i < 48; i++)
  {
    buff[i] = EEPROM.read(startAddress + i);
  }
  if (_lc[0] != NULL)
  {
    for (int i = 0; i < 30; i += 2)
    {
      bool isCompleted = (buff[i]) >> 7;
      uint16_t clientNumber = ((buff[i] << 8) & ~(1 << 15)) + (buff[i + 1]);
      // Serial.printf("%u-%u-%u,", i, clientNumber, isCompleted);
      uint8_t display = ((i / 2) % 16) + 1; // т.к первый это время, делаем + 1
      if (clientNumber > 0 && clientNumber < 9999)
      {
        _lc[0]->setDisplay(display, clientNumber);
        _lc[0]->completed[display] = isCompleted;
      }
    }
  }

  if (_lc[1] != NULL)
  {
    for (int i = 30; i < 46; i += 2)
    {
      bool isCompleted = (buff[i]) >> 7;
      uint16_t clientNumber = ((buff[i] << 8) & ~(1 << 15)) + (buff[i + 1]);

      // Serial.printf("%u-%u-%u,", i, clientNumber, isCompleted);
      uint8_t display = (i / 2) - 15; // т.к первый это время, делаем + 1
      if (clientNumber > 0 && clientNumber < 9999)
      {
        _lc[1]->setDisplay(display, clientNumber);
        _lc[1]->completed[display] = isCompleted;
      }
    }
  }
}
//static SocketIoClient * ws = NULL;
void setup()
{
  EEPROM.begin(512);
  static OTAUpdateProcess otaUpdate(sched, HIGH_PRIORITY, 250, "0819");
  otaUpdate.add(true);
  Serial.begin(9600);
  ArduinoOTA.begin();

  setupWiFiAP();
  //WiFi.mode(WIFI_AP);//WIFI_STA);AP
  WiFi.begin(SSID, PASSWORD);

  for (int i = 0; i < 5; i++)
  {
    ArduinoOTA.handle();
    delay(100);
  }
  if (MAX7219_COUNT < 9)
    lc[0] = new LedArray(1, LEDTABLE1_DIN_pin, LEDTABLE1_CLK_pin, LEDTABLE1_LOAD_pin, MAX7219_COUNT);
  else
  {
    lc[0] = new LedArray(1, LEDTABLE1_DIN_pin, LEDTABLE1_CLK_pin, LEDTABLE1_LOAD_pin, 8);
    lc[1] = new LedArray(1, LEDTABLE2_DIN_pin, LEDTABLE2_CLK_pin, LEDTABLE2_LOAD_pin, MAX7219_COUNT - 8);
  }
  lc[0]->setChar(0, 0, 'L');
  lc[0]->setChar(0, 1, '0');
  lc[0]->setChar(0, 2, 'A');
  lc[0]->setChar(0, 3, 'd');
  loadLedArrayFromEEPROM(lc);
  for (int i = 0; i < 5; i++)
  {
    ArduinoOTA.handle();
    delay(100);
  }
  lc[0]->setChar(0, 0, 'c');
  lc[0]->setChar(0, 1, '0');
  lc[0]->setChar(0, 2, 'n');
  lc[0]->setChar(0, 3, 'n');
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
#ifdef DEBUG_SERIAL
    Serial.println("Connection Failed! Rebooting...");
#endif
    lc[0]->setChar(0, 0, 'E');
    lc[0]->setChar(0, 1, '0');
    lc[0]->setChar(0, 2, 'C');
    lc[0]->setChar(0, 3, '1');
    delay(5000);
    ESP.restart();
  }
  Serial.println(WiFi.localIP());

  static TcpSocketProcess tcp(sched, LOW_PRIORITY, 100, lc);
  p_tcp = &tcp;

  static ClockProcess clock(sched, HIGH_PRIORITY, 1000, lc);
  Serial.println(3);
  static CheckedOrdersWatcherProcess watcher_orders(sched, HIGH_PRIORITY, 250, lc);

//static LedArrayWatcherProcess ledArrWather(sched, LOW_PRIORITY, 10000, MAX7219_COUNT, &clock, lc);
#ifdef BTN_WATCHER_ENABLE
  static ButtonsArrayWatcherProcess btnWather(sched, HIGH_PRIORITY, 100, PCF8574AP_SDA_PIN, PCF8574APSCL_PIN, SN74HC595_CLOCK_PIN, SN74HC595_LATCH_PIN, SN74HC595_DATA_PIN, lc, p_ws, p_tcp);
#endif

  //console.begin(p_ws);
  //ws->add(true);

  //socket.add(true);
  //  Serial.println((int)&socket);

  //ledArrWather.add(true);
  clock.add(true);
  // ws.add(true);
  tcp.add(true);
  Serial.println(4);
  char ipno2[64];
  IPAddress ipno = WiFi.localIP();
  // sprintf(ipno2, "{\"ip\":\"%d.%d.%d.%d\"}", ipno[0], ipno[1], ipno[2], ipno[3]);
#ifdef DEBUG_SERIAL
  for (int i = 0; i < 64; i++)
  {
    if (ipno2[i] == '\0')
    {
      Serial.println("out");
      break;
    }
    Serial.printf("%d ", ipno2[i]);
  }
  Serial.println(ipno2);
#endif
  // p_ws->json((char *)(&ipno2), CONNECT);
//console.log(WiFi.localIP());
#ifdef DEBUG_SERIAL
  Serial.println(5);
  Serial.println("wait for rx-tx support");
#endif
  watcher_orders.add(true);
#ifdef BTN_WATCHER_ENABLE
  btnWather.add(true);
#endif
}
void loop()
{
  // lc[1]->setDisplay(8,1234);
  // lc[1]->setDisplay(7,1234);
  // lc[1]->setDisplay(6,1234);
  // lc[1]->setDisplay(5,1234);
  // lc[1]->setDisplay(4,1234);
  sched.run();
}
