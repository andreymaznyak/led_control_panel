#ifndef _CHECKED_ORDERS_WATCHER_PROCESS_
#define _CHECKED_ORDERS_WATCHER_PROCESS_

#include <ArduinoOTA.h>
#include <ProcessScheduler.h>
#include "LedArray.cpp"

class CheckedOrdersWatcherProcess : public Process
{
public:
    // Call the Process constructor
    CheckedOrdersWatcherProcess(Scheduler &manager, ProcPriority pr, unsigned int period, LedArray * lc[2] = NULL)
        :  Process(manager, pr, period)
        {
          if(lc != NULL)
            for(uint8_t i = 2; i--;){
              _lc[i] = lc[i];
            }
        }

protected:
    virtual void setup()
    {

      Serial.println("CheckedOrdersWatcherProcess started");
    }

     // Undo setup()
    virtual void cleanup()
    {

    }

    // Create our service routine
    virtual void service()
    {
      for(int i = 0; i < 2; i++){
        for(int j = 0; j < 16; j++){
          if(i == 0 && j == 0)
            break;
          if(_lc[i]->completed[j]){
            if(_lc[i]->tick[j]){
              _lc[i]->setDisplay(j, 8888, 0xFF, false);
            }else{
              _lc[i]->setDisplay(j, _lc[i]->val[j], 0, false);
            }
            _lc[i]->tick[j] = !_lc[i]->tick[j];
          }
        }
      }
    }
private:
  LedArray * _lc[2];
};
#endif
