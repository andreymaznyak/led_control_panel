#ifndef TCP_SOCKET_PROCESS
#define TCP_SOCKET_PROCESS

#include <Arduino.h>
#include <ProcessScheduler.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "../config.h"
#include "LedArray.cpp"
/**
 * Протокол обмена
 * 
 * Сообщение От сервера
 * 
 * Общая информация:
 *  
 * Описание пакетов клиента:
 * 
 * CLIENT_INFO - id сообщения 1
 * Событие происходит при открытии клиенсткого сокета и при запросе информации
 * клиент отсылает свой идентификатор(номер стойки), ip - Адресс ip-v4, 
 * getChipId, getFreeHeap
 * Размер пакета 16 байт
 * | 1 байт - id сообщения | 2 - 5 байты - id aдрес | 6 байт - номер стойки | 7 - 10 байт - id чипа | 11 - 14 байт размер свободной памяти | 15 - 16 байт версия прошивки |
 * 
 * PRESS_BUTTON - id сообщения 2
 * Если нажата кнопка на сервер отсылается индекс нажатой кнопки
 * Размер пакета 2 байта
 * | 1 байт - id сообщения | 2 байт - данные о номере |
 * Данные о номере:
 * | Первые 5 бит - индекс номера | 6-8 бит не используется |
 * 
 * 
 * Описание пакетов от сервера:
 * 
 * SET_DIGIT_COMPLETED - id сообщения = 3
 * Пакет говорящий что какой то номер должен замигать, или наоборот перестать мигать
 * Размер пакета 2 байта
 * | 1 байт - id сообщения | 2 байт - данные о номере |
 * Данные о номере:
 * | Первые 5 бит - индекс номера | 6-7 бит не используется | 8 бит - мигать или нет |
 * 
 * UPDATE_ARR_NUMBERS_AND_COMPLETED_STATUS - id сообщения = 4
 * Пакет передающий всю информацию о номерах которые нужно отображать
 * Размер пакета 49 байт 
 * | 1 байт - id сообщения | 2 - 49 - данные с о клиентах |  
 * Данные о клиентах 24 блока по 2 байта
 * Данные о клиенте:
 * | Первые 14 бит - число - номер клиента | 15 бит не используется | 16 бит - мигать или нет |
 * 
 * RESTART - id сообщения 5
 * Пакет говорящий стойке что бы она перезагрузилась
 * Размер пакета 1 байт
 * | 1 байт - id сообщения |
 * 
 * GET INFO - id сообщения 6
 * Пакет запрашивающий системные данные ( размер свободной памяти и т.п. )
 * Размер пакета 1 байт
 * | 1 байт - id сообщения |
 * 
 **/

enum CLIENT_EVENTS_V2
{
    CLIENT_INFO_V2 = 1, // Событие соединения клиента с сервером
    PRESS_BUTTON_V2 = 2 // Событие нажатия кнопки на клиенте
};

enum SERVER_EVENTS_V2
{
    SET_DIGIT_COMPLETED_V2 = 3,                     // требуется утановить какой то номер как completed (что бы мигал)
    UPDATE_ARR_NUMBERS_AND_COMPLETED_STATUS_V2 = 4, // требуется обновить номера клиентов на светодиодных табло и установить как completed (что бы мигали)
    RESTART_V2 = 5,
    GET_INFO_V2 = 6
};

struct ClientInfoPackage
{
    uint8_t messageId = CLIENT_INFO_V2;
    uint8_t ip0 = 0;
    uint8_t ip1 = 0;
    uint8_t ip2 = 0;
    uint8_t ip3 = 0;
    uint8_t deviceId = 0;
    uint32_t chipId = 0;
    uint32_t freeHeapSize = 0;
    uint16_t softwareVersion = 0;
};

struct PressButtonPackage
{
    uint8_t messageId = PRESS_BUTTON_V2;
    uint8_t pressButtonIndex = 0;
};

struct GetInfoPackage
{
    uint8_t messageId = GET_INFO_V2;
};

struct RestartPackage
{
    uint8_t messageId = RESTART_V2;
};

struct UpdateNumbersPackage
{
    uint8_t messageId = UPDATE_ARR_NUMBERS_AND_COMPLETED_STATUS_V2;
    uint8_t numbers[48];
};

struct SetDigitCompletedPackage
{
    uint8_t messageId = SET_DIGIT_COMPLETED_V2;
    uint8_t index = 0;
};

/**
* @description Процесс отвечает за принятие, отправку и обработку сообщений полученных
*              по websocket соединению с nodejs сервером
*
*/
class TcpSocketProcess : public Process
{
  public:
    // Call the Process constructor
    TcpSocketProcess(Scheduler &manager, ProcPriority pr, unsigned int period, LedArray *lc[2] = NULL)
        : Process(manager, pr, period)
    {
        if (lc != NULL)
        {
            for (uint8_t i = 2; i--;)
            {
                _lc[i] = lc[i];
            }
        }
    }
    void send(char *message)
    {
    }
    void sendDeviceInfo()
    {
        struct ClientInfoPackage info = getDeviceInfo();
        client.write((uint8_t *)&(info), 16);
    }
    void sendButtonPress(uint8_t index)
    {
        struct PressButtonPackage info;
        info.messageId = PRESS_BUTTON_V2;
        info.pressButtonIndex = index;
        client.write((uint8_t *)&(info), 2);
    }

  protected:
    virtual void setup()
    {
        client.setNoDelay(true);
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
            if (client.available())
            {
                const uint8_t BUFF_LENGTH = 128;
                uint8_t buff[BUFF_LENGTH];
                for (int i = 0; i < BUFF_LENGTH; i++)
                {
                    buff[i] = 0;
                }
                int i = client.read((uint8_t *)&buff, BUFF_LENGTH);
                for (int i = 0; i < BUFF_LENGTH; i++)
                {
                    Serial.printf("%u ", buff[i]);
                }
                uint8_t cur = 0;
                while (cur < BUFF_LENGTH)
                {
                    switch (buff[cur])
                    {
                    case SET_DIGIT_COMPLETED_V2:
                        setCompletedNumber(buff[cur + 1]);
                        sendButtonPress(buff[cur + 1]);
                        cur += 2;
                        break;
                    case GET_INFO_V2:
                        sendDeviceInfo();
                        cur++;
                        break;
                    case RESTART_V2:
                        ESP.restart();
                        Serial.println("restart");
                        cur++;
                        break;
                    case UPDATE_ARR_NUMBERS_AND_COMPLETED_STATUS_V2:
                        updateNumbers((void *)(buff + cur));
                        cur += 49;
                        break;
                    default:
                        cur++;
                    }
                }
            }
        }
        else
        {
            // Serial.printf("Client disconnected. wait to reconnect %d", sleep);
            // delay(3000);
            // ESP.restart();
            connect();
        }
    }

  private:
    void connect()
    {
        // Connect to the websocket server
        if (client.connect(TCP_SOCKET_HOST, TCP_SOCKET_PORT))
        {
            tcp_connected = true;
            Serial.println("Connected");
            sendDeviceInfo();
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
    }

    void saveToEEPROM(void *pnt)
    {
        uint8_t *valPnt = (uint8_t *)pnt;
        uint8_t buff[48]; // Буффер для данных стоек
        const int startAddress = START_ADDRESS;
        bool equals = true;
        for (uint8_t i = 0; i < 48; i++)
        {
            buff[i] = EEPROM.read(startAddress + i);
            uint8_t val = *(valPnt + i);
            if (buff[i] != val) // ещем неравный байт
            {
                equals = false;
                break;
            }
        }
        if (equals == false)
        {
            for (int i = 0; i < 48; i++)
            {
                uint8_t val = *(valPnt + i);
                EEPROM.write(startAddress + i, val);
            }
            EEPROM.commit();
        }
    }

    void updateNumbers(void *buff)
    {

        struct UpdateNumbersPackage *info = 0;
        info = (struct UpdateNumbersPackage *)buff;
        _lc[0]->clearDislays();
        _lc[1]->clearDislays();
        for (int i = 0; i < 30; i += 2)
        {
            bool isCompleted = (info->numbers[i]) >> 7;
            uint16_t clientNumber = ((info->numbers[i] << 8) & ~(1 << 15)) + (info->numbers[i + 1]);
            // Serial.printf("%u-%u-%u,", i, clientNumber, isCompleted);
            uint8_t display = ((i / 2) % 16) + 1; // т.к первый это время, делаем + 1
            if (clientNumber > 0)
            {
                _lc[0]->setDisplay(display, clientNumber);
                _lc[0]->completed[display] = isCompleted;
            }
        }
        for (int i = 30; i < 48; i += 2)
        {
            bool isCompleted = (info->numbers[i]) >> 7;
            uint16_t clientNumber = ((info->numbers[i] << 8) & ~(1 << 15)) + (info->numbers[i + 1]);

            // Serial.printf("%u-%u-%u,", i, clientNumber, isCompleted);
            uint8_t display = (i / 2) - 15; // т.к первый это время, делаем + 1
            if (clientNumber > 0)
            {
                _lc[1]->setDisplay(display, clientNumber);
                _lc[1]->completed[display] = isCompleted;
            }
        }
        saveToEEPROM(&(info->numbers));
    }
    void setCompletedNumber(uint8_t indexInfo)
    {
        bool isCompleted = indexInfo >> 7;
        uint8_t indexValue = indexInfo & ~(1 << 7);

        int index = indexValue / 16;
        int display = (indexValue % 16) + (1 - index);
        _lc[index]->completed[display] = (isCompleted == 1);
    }

    ClientInfoPackage getDeviceInfo()
    {
        struct ClientInfoPackage info;
        info.messageId = CLIENT_INFO_V2;
        info.ip0 = WiFi.localIP()[0];
        info.ip1 = WiFi.localIP()[1];
        info.ip2 = WiFi.localIP()[2];
        info.ip3 = WiFi.localIP()[3];
        info.freeHeapSize = ESP.getFreeHeap();
        info.chipId = ESP.getChipId();
        info.deviceId = DEVICE_ID;
        info.softwareVersion = SOFTVARE_VERSION;
        return info;
    }
    bool tcp_connected = false;
    uint8_t sleep = 0;
    WiFiClient client;
    LedArray *_lc[2];
};

#endif
