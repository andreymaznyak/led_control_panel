// #ifndef _SOCKET_IO_CLIENT_
// #define _SOCKET_IO_CLIENT_
//
// #include <Arduino.h>
// #ifndef ESP8266
// #define ESP8266
// #endif
// #include <SocketIOClient.h>
// #include <ArduinoJson.h>
//
//
// #include <ProcessScheduler.h>
//
// extern String RID;
// extern String Rname;
// extern String Rcontent;
//
// class SocketIoClient : public Process
// {
// public:
//     // Call the Process constructor
//     SocketIoClient(Scheduler &manager, ProcPriority pr, unsigned int period, uint8_t host_ip[], const int host_port = 1337)
//         :  Process(manager, pr, period)
//         {
//           // Port defaults to 8266
//           _port = host_port;
//           sprintf(ip,"%d.%d.%d.%d", host_ip[0],host_ip[1],host_ip[2],host_ip[3]);
//
//         }
//     uint8_t sendMessage(String message){
//       if(connected){
//           //client.heartbeat(0);
//
//           Serial.println(JSON);//"orders1c?secret=magic1cCookie&limit=1&populate=false"
//           //client.sendJSON("orders1c",JSON);
//           //client.send("message", "get /LedArray/test", "Time please?");
//           root["url"] = "/DeviceMessages/message?text=" + message + "secret=magic1cCookie&";// /test"/orders1c?limit=10&populate=false";
//           root.printTo(JSON);
//           //Serial.println(JSON);
//           client.sendJSON("get", JSON);
//       }else{
//         if (!client.connect(ip, _port)) {
//             Serial.println("connection failed");
//             return -1;
//         }
//         connected = true;
//         return 1;
//       }
//     }
// protected:
//     virtual void setup()
//     {
//
//
//
//       delay(10);
//
//
//       if (!client.connect(ip, _port)) {
//         Serial.println("connection failed");
//         return;
//       }
//       connected = true;
//       if (client.connected()){
//           client.send("connection", "message", "Connected !!!!");
//       }
//
//       Serial.println("SocketIoClient started");
//     }
//
//      // Undo setup()
//     virtual void cleanup()
//     {
//
//     }
//
//     // Create our service routine
//     virtual void service()
//     {
//       client.monitor();
//       // if(connected){
//       //   unsigned long currentMillis = millis();
//       //   if (currentMillis - previousMillis > interval){
//       //     previousMillis = currentMillis;
//       //     //client.heartbeat(0);
//       //   Serial.println(JSON);//"orders1c?secret=magic1cCookie&limit=1&populate=false"
//       //   //client.sendJSON("orders1c",JSON);
//       // //  client.send("message", "get /LedArray/test", "Time please?");
//       //     client.sendJSON("get", JSON);
//       //
//       //     lastsend = millis();
//       //   }
//       //   if (client.monitor()){
//       //     lastreply = millis();
//       //     Serial.println(RID);
//       //     if (RID == "atime" && Rname == "time"){
//       //       Serial.print("Il est ");
//       //       Serial.println(Rcontent);
//       //     }
//       //   }
//       // }else{
//       //   if (!client.connect(ip, _port)) {
//       //     Serial.println("connection failed");
//       //     return;
//       //   }
//       //   connected = true;
//       //   if (client.connected()){
//       //       client.send("connection", "message", "Connected !!!!");
//       //   }
//       //
//       //   Serial.println("SocketIoClient started");
//       // }
//
//     }
// private:
//     bool connected = false;
//     SocketIOClient client;
//     char ip[16];
//     int _port;
//     unsigned long previousMillis = 0;
//     long interval = 5000;
//     unsigned long lastreply = 0;
//     unsigned long lastsend = 0;
//     String JSON;
//     StaticJsonBuffer<200> jsonBuffer;
//     JsonObject& root = jsonBuffer.createObject();
// };
// #endif
