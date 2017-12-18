#ifndef WEB_SOCKET_PROCESS
#define WEB_SOCKET_PROCESS

#include <Arduino.h>
#include <ProcessScheduler.h>
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

#include "LedArray.cpp"

#include "config.h"

enum CLIENT_EVENTS
{
  CONNECT = 1,        // Событие соединения клиента с сервером
  CLIENT_MESSAGE = 2, // Событие при собщении от клиента
  PRESS_BUTTON = 3,   // Событие нажатия кнопки на клиенте

};

enum SERVER_EVENTS
{
  UPDATE_ARR_NUMBERS = 1,                     // требуется обновить номера клиентов на светодиодных табло
  SERVER_MESSAGE = 2,                         // кастомное сообщение от сервера
  SET_DIGIT_COMPLETED = 3,                    // требуется утановить какой то номер как completed (что бы мигал)
  UPDATE_ARR_NUMBERS_AND_COMPLETED_STATUS = 4 // требуется обновить номера клиентов на светодиодных табло и установить как completed (что бы мигали)
};

/**
* @description Процесс отвечает за принятие, отправку и обработку сообщений полученных
*              по websocket соединению с nodejs сервером
*
*/
class WebSocketProcess : public Process
{
public:
  // Call the Process constructor
  WebSocketProcess(Scheduler &manager, ProcPriority pr, unsigned int period, LedArray *lc[2] = NULL)
      : Process(manager, pr, period)
  {
    Serial.println("trying to init lc");
    if (lc != NULL)
      for (uint8_t i = 2; i--;)
      {
        _lc[i] = lc[i];
      }
  }
  void send(char *message)
  {
    sendingMessagesIndex++;
    if (sendingMessagesIndex > 9)
      sendingMessagesIndex = 1;
    memset(sendingMessages[sendingMessagesIndex - 1], 0, sizeof(sendingMessages[sendingMessagesIndex - 1])); // очистка буфера для вывода.
    strcpy(sendingMessages[sendingMessagesIndex - 1], message);
#ifdef DEBUG_SERIAL
    Serial.printf("after copy %s <- %s \n", sendingMessages[sendingMessagesIndex - 1], message);
    Serial.printf("Sending sendingMessagesIndex %d \n", sendingMessagesIndex);
#endif
    sendAllMessages();
  }
  void message(char *message, CLIENT_EVENTS action)
  {
    char JSON[64];
    sprintf(JSON, "{\"action\":\"%d\",\"message\":\"%s\"}\n", action, message);
    send((char *)&JSON);
  }
  void json(char *message, CLIENT_EVENTS action)
  {
    char JSON[64];
    sprintf(JSON, "{\"action\":\"%d\",\"message\":%s}\n", action, message);
    send((char *)&JSON);
  }

protected:
  virtual void setup()
  {
    connect();
  }

  // Undo setup()
  virtual void cleanup()
  {
  }

  // Create our service routine
  virtual void service()
  {
    String data;
    if (client.connected())
    {

      webSocketClient.getData(data);

      if (data.length() > 0)
      {

#ifdef DEBUG_SERIAL
        Serial.print("Received data: ");
        Serial.println(data);
#endif

        StaticJsonBuffer<2048> jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(data);

        if (!root.success())
        {
#ifdef DEBUG_SERIAL
          Serial.println("parseObject() failed");
          return;
#endif
          //Ошибка парсинга JSON
          _lc[0]->setChar(0, 0, 'E');
          _lc[0]->setChar(0, 1, '0');
          _lc[0]->setChar(0, 2, '0');
          _lc[0]->setChar(0, 3, '1');
          delay(1000);
        }

        int action = root["action"];

        switch (action)
        {
        case SERVER_MESSAGE:

          break;
          //TODO update led array numbers
        case SET_DIGIT_COMPLETED:
          if (true)
          {
            String number = root["number"];
            int value = root["value"];
            int index = number.toInt() / 16;
            int display = (number.toInt() % 16) + (1 - index);

#ifdef DEBUG_SET_COMPLETED_EVENT
            data = data + " ind " + index + " display " + display + " number " + number + " value " + (bool)(value == 1);
            char buf[128];
            data.toCharArray(buf, 128);
            message(buf, CLIENT_MESSAGE);

#endif
            _lc[index]->completed[display] = (value == 1);
          }
          break;
        case UPDATE_ARR_NUMBERS:
          _lc[0]->clearDislays();
          _lc[1]->clearDislays();
          // первая цепочка цифровых табло
          for (int i = 0; i < 15; i++)
          {

            int val = root["numbers_arr"][i];
            uint8_t display = (i % 16) + 1; // т.к первый это время делаем + 1
            if (val > 0)
            {
              _lc[0]->setDisplay(display, val);
            }
          }
          // вторая цепочка цифровых табло
          for (int i = 15; i < 23; i++)
          {

            int val = root["numbers_arr"][i];
            uint8_t display = i - 15;
            if (val > 0)
            {
              _lc[1]->setDisplay(display, val);
            }
          }
          break;
        case UPDATE_ARR_NUMBERS_AND_COMPLETED_STATUS:
          _lc[0]->clearDislays();
          _lc[1]->clearDislays();

          // первая цепочка цифровых табло
          for (int i = 0; i < 15; i++)
          {

            int order_id = root["numbers_arr"][i]["order_id"];
            bool is_checked = root["numbers_arr"][i]["is_checked"];
            uint8_t display = (i % 16) + 1; // т.к первый это время, делаем + 1
            if (order_id > 0)
            {
              _lc[0]->setDisplay(display, order_id);
              _lc[0]->completed[display] = is_checked;
            }
          }
          // вторая цепочка цифровых табло
          for (int i = 15; i < 23; i++)
          {
            int order_id = root["numbers_arr"][i]["order_id"];
            bool is_checked = root["numbers_arr"][i]["is_checked"];
            uint8_t display = i - 15;
            if (order_id > 0)
            {
              _lc[1]->setDisplay(display, order_id);
              _lc[1]->completed[display] = is_checked;
            }
          }
          break;
        default:
#ifdef DEBUG_SERIAL
          Serial.println("unknown action");
#endif
          //Ошибка неизвестное дейсвие
          _lc[0]->setChar(0, 0, 'E');
          _lc[0]->setChar(0, 1, '0');
          _lc[0]->setChar(0, 2, '0');
          _lc[0]->setChar(0, 3, '2');
          delay(1000);
        }
      }
      sendAllMessages();
    }
    else
    {
#ifdef DEBUG_SERIAL
      Serial.printf("Client disconnected. wait to reconnect %d", sleep);
#endif
      _lc[0]->setChar(0, 0, 'E');
      _lc[0]->setChar(0, 1, '4');
      _lc[0]->setChar(0, 2, '0');
      _lc[0]->setChar(0, 3, '4');
      ESP.restart();
      delay(100);

      connect();
    }
  }

private:
  void sendAllMessages()
  {
    if (client.connected())
    {
      //Serial.printf("Seding index %d \n", sendingMessagesIndex);
      for (uint8_t index = sendingMessagesIndex - 1; sendingMessagesIndex > 0; sendingMessagesIndex--, index = sendingMessagesIndex - 1)
      {
#ifdef DEBUG_SERIAL
        Serial.printf("send message: %s \n", sendingMessages[index]);
#endif
        webSocketClient.sendData(sendingMessages[index]);
        memset(sendingMessages[index], 0, sizeof(sendingMessages[index])); // очистка буфера для вывода.
      }
    }
  }
  void connect()
  {
    // Connect to the websocket server
    if (client.connect("192.168.100.172", 1447))
    {
      tcp_connected = true;
#ifdef DEBUG_SERIAL
      Serial.println("Connected");
#endif
    }
    else
    {
#ifdef DEBUG_SERIAL
      Serial.println("TCP Connection failed.");
#endif
      _lc[0]->setChar(0, 0, 'E');
      _lc[0]->setChar(0, 1, '4');
      _lc[0]->setChar(0, 2, '0');
      _lc[0]->setChar(0, 3, '5');
      delay(100);
      tcp_connected = false;
    }

    // Handshake with the server
    webSocketClient.path = DEVICE_ID_STR;
    webSocketClient.host = "192.168.100.172:1447";
    if (webSocketClient.handshake(client))
    {
      ws_connected = true;
#ifdef DEBUG_SERIAL
      Serial.println("Handshake successful");
#endif
    }
    else
    {
      ws_connected = false;
#ifdef DEBUG_SERIAL
      Serial.println("Handshake failed.");
#endif
      _lc[0]->setChar(0, 0, 'E');
      _lc[0]->setChar(0, 1, '4');
      _lc[0]->setChar(0, 2, '0');
      _lc[0]->setChar(0, 3, '6');
      delay(100);
    }
#ifdef DEBUG_SERIAL
    Serial.println("WEBSOCKET started");
#endif
  }
  bool tcp_connected = false;
  bool ws_connected = false;
  uint8_t sleep = 0;
  String host;
  String path;
  WebSocketClient webSocketClient;
  WiFiClient client;
  char sendingMessages[10][64];
  uint8_t sendingMessagesIndex = 0;
  LedArray *_lc[2];
};

#endif
