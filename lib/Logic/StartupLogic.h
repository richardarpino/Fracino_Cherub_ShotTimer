#ifndef STARTUP_LOGIC_H
#define STARTUP_LOGIC_H

#include "../Interfaces/ISensor.h"
#include "../Interfaces/ISwitch.h"

class StartupLogic {
public:
    StartupLogic(ISwitch* wifi, unsigned long holdDurationMs = 3000);
    
    void update();
    bool isComplete();
    bool justFinished();

private:
    ISwitch* _wifi;
    bool _isComplete;
    bool _lastComplete;
    unsigned long _startTime;
    unsigned long _holdDurationMs;
};

#endif
