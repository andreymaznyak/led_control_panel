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
      #ifdef DEBUG_SERIAL
      //Serial.println("tick watcher orders");
      #endif

      for(int j = 1; j < 16; j++){
        //Serial.printf("_lc[%d]->completed[%d] = %d  val = %d \n", 0, j, _lc[0]->completed[j],  _lc[0]->val[j]);
        if( (_lc[0]->completed[j]) && (_lc[0]->val[j] > 0) ){
          if(_lc[0]->tick[j]){
            for(int k = 0; k < 4; k++){
                _lc[0]->setChar((j/2),k + (j % 2) * 4,' ',false);
            }
            //_lc[0]->setDisplay(j, 8888, 0xFF, false);
          }else{
            _lc[0]->setDisplay(j, _lc[0]->val[j], 0, false);
          }
          _lc[0]->tick[j] = !( _lc[0]->tick[j] );

        }
      }

      for(int j = 0; j < 8; j++){
        //Serial.printf("_lc[%d]->completed[%d] = %d  val = %d \n", 1, j, _lc[1]->completed[j],  _lc[1]->val[j]);
        if( ((_lc[1]->completed[j]) && (_lc[0]->val[j] > 0) ) /*&& (_lc[i]->val[j] > 0)*/ ){
          if(_lc[1]->tick[j]){
            _lc[1]->setDisplay(j, 8888, 0xFF, false);
          }else{
            _lc[1]->setDisplay(j, _lc[1]->val[j], 0, false);
          }
          _lc[1]->tick[j] = !( _lc[1]->tick[j] );

        }
      }

    }
private:
  LedArray * _lc[2];
};
#endif
