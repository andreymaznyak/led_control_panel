#ifndef _OTA_UPDATE_PROCESS_
#define _OTA_UPDATE_PROCESS_

#include <ArduinoOTA.h>
#include <ProcessScheduler.h>
/**
* @description Класс для процесса обновления через wifi
*
*/
class OTAUpdateProcess : public Process
{
public:
    // Call the Process constructor
    OTAUpdateProcess(Scheduler &manager, ProcPriority pr, unsigned int period, const char* password = "", const int port = 8266)
        :  Process(manager, pr, period)
        {
          // Port defaults to 8266
          ArduinoOTA.setPort(port);

          // Hostname defaults to esp8266-[ChipID]
          ArduinoOTA.setHostname("first_display_table");

          // No authentication by default
          if(password[0] != '\0')
            ArduinoOTA.setPassword((const char *) "0819");

          ArduinoOTA.onStart([]() {
            Serial.println("Start");
          });
          ArduinoOTA.onEnd([]() {
            Serial.println("\nEnd");
          });
          ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
          });
          ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
          });
        }

protected:
    virtual void setup()
    {
      ArduinoOTA.begin();
      Serial.println("OTAUpdateProcess started");
    }

     // Undo setup()
    virtual void cleanup()
    {

    }

    // Create our service routine
    virtual void service()
    {
      ArduinoOTA.handle();
    }
};
#endif
