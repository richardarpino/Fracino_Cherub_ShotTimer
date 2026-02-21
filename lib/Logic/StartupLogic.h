#ifndef STARTUP_LOGIC_H
#define STARTUP_LOGIC_H

#include "../Interfaces/ISensor.h"

class StartupLogic {
public:
    StartupLogic(ISensor* wifiSensor, unsigned long holdDurationMs = 3000);
    
    void update();
    bool isComplete();
    bool justFinished();

private:
    ISensor* _wifi;
    bool _isComplete;
    bool _lastComplete;
    unsigned long _startTime;
    unsigned long _holdDurationMs;
};

#endif
