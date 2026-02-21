#ifndef STARTUP_LOGIC_H
#define STARTUP_LOGIC_H

#include "../Interfaces/ISensor.h"
#include "../Interfaces/ISwitch.h"
#include "../Interfaces/IOTAService.h"

class StartupLogic {
public:
    enum class State {
        SEARCHING_WIFI,
        WIFI_STABLE,
        OTA_STARTING,
        READY
    };

    StartupLogic(ISwitch* wifi, IOTAService* ota, unsigned long holdDurationMs = 3000);
    
    void update();
    State getState() const { return _state; }

private:
    ISwitch* _wifi;
    IOTAService* _ota;
    State _state;
    bool _isComplete;
    bool _lastComplete;
    unsigned long _startTime;
    unsigned long _holdDurationMs;
};

#endif
