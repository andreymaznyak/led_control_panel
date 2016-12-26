#ifndef WEB_SOCKET_PROCESS
#define WEB_SOCKET_PROCESS

#include <Arduino.h>
#include <ProcessScheduler.h>
#include <ESP8266WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

#include "LedArray.cpp"

#include "config.h"

enum CLIENT_EVENTS {
  CONNECT = 1,
  CLIENT_MESSAGE = 2,
  PRESS_BUTTON = 3,

};

enum SERVER_EVENTS {
  UPDATE_ARR_NUMBERS = 1,
  SERVER_MESSAGE = 2
};



class WebSocketProcess : public Process
{
public:
    // Call the Process constructor
    WebSocketProcess(Scheduler &manager, ProcPriority pr, unsigned int period, LedArray * lc[2] = NULL)
        :  Process(manager, pr, period)
        {
          Serial.println("trying to init lc");
          if(lc != NULL)
            for(uint8_t i = 2; i--;){
              _lc[i] = lc[i];
            }
        }
        void send(char * message){
          sendingMessagesIndex++;
          if(sendingMessagesIndex > 9) sendingMessagesIndex = 1;
          memset(sendingMessages[sendingMessagesIndex - 1], 0, sizeof(sendingMessages[sendingMessagesIndex - 1]));          // очистка буфера для вывода.
          strcpy(sendingMessages[sendingMessagesIndex - 1], message);
          #ifdef DEBUG_SERIAL
          Serial.printf("after copy %s <- %s \n", sendingMessages[sendingMessagesIndex - 1], message);
          Serial.printf("Sending sendingMessagesIndex %d \n", sendingMessagesIndex);
          #endif
          sendAllMessages();

        }
        void message(char * message, CLIENT_EVENTS action){
          char JSON[64];
          sprintf(JSON,"{\"action\":\"%d\",\"message\":\"%s\"}\n", action, message);
          send((char*)&JSON);
        }
        void json(char * message, CLIENT_EVENTS action){
          char JSON[64];
          sprintf(JSON,"{\"action\":\"%d\",\"message\":%s}\n", action, message);
          send((char*)&JSON);
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
      if (client.connected()) {

        webSocketClient.getData(data);
        if (data.length() > 0) {

          #ifdef DEBUG_SERIAL
          Serial.print("Received data: ");
          Serial.println(data);
          #endif

          StaticJsonBuffer<200> jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(data);

          if (!root.success())
          {
            #ifdef DEBUG_SERIAL
            Serial.println("parseObject() failed");
            return;
            #endif
          }

          uint16_t action = root["action"];

          switch(action){
            case SERVER_MESSAGE:
              //TODO update led array numbers

              break;
            default:
            #ifdef DEBUG_SERIAL
            Serial.println("unknown action");
            #endif
          }


        }
        sendAllMessages();

      }else{
        #ifdef DEBUG_SERIAL
        Serial.printf("Client disconnected. wait to reconnect %d", sleep);
        #endif
        if(!sleep){
          connect();
          sleep += 10;
        }
        sleep--;
      }

    }

private:
  void sendAllMessages(){
    if (client.connected()) {
      //Serial.printf("Seding index %d \n", sendingMessagesIndex);
      for(uint8_t index = sendingMessagesIndex - 1;sendingMessagesIndex > 0;sendingMessagesIndex--,index = sendingMessagesIndex - 1){
        #ifdef DEBUG_SERIAL
        Serial.printf("send message: %s \n", sendingMessages[index]);
        #endif
        webSocketClient.sendData(sendingMessages[index]);
        memset(sendingMessages[index], 0, sizeof(sendingMessages[index]));          // очистка буфера для вывода.
      }
    }
  }
  void connect(){
    // Connect to the websocket server
    if (client.connect("192.168.100.141", 1447)) {
      tcp_connected = true;
      #ifdef DEBUG_SERIAL
      Serial.println("Connected");
      #endif
    } else {
      #ifdef DEBUG_SERIAL
      Serial.println("Connection failed.");
      #endif
      tcp_connected = false;
    }

    // Handshake with the server
    webSocketClient.path = "/device/0";
    webSocketClient.host = "192.168.100.141:1447";
    if (webSocketClient.handshake(client)) {
      ws_connected = true;
      #ifdef DEBUG_SERIAL
      Serial.println("Handshake successful");
      #endif
    } else {
      ws_connected = false;
      #ifdef DEBUG_SERIAL
      Serial.println("Handshake failed.");
      #endif

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
  LedArray * _lc[2];

};

#endif
