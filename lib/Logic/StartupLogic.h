#ifndef STARTUP_LOGIC_H
#define STARTUP_LOGIC_H

#include "../Interfaces/ISensor.h"
#include "../Interfaces/ISwitch.h"

class ISwitchProvider;

class StartupLogic : public ISwitch {
public:
    StartupLogic(ISwitchProvider* provider, unsigned long holdDurationMs = 3000);
    
    void update() override;
    
    // ISwitch implementation
    bool isActive() const override;
    bool justStarted() const override;
    bool justStopped() const override;

private:
    enum class State {
        SEARCHING_WIFI,
        WIFI_STABLE,
        OTA_STARTING,
        READY
    };

    ISwitch* _wifi;
    ISwitchProvider* _provider;
    State _state;
    bool _isActive;
    bool _justStarted;
    bool _justStopped;
    bool _lastActive;
    unsigned long _startTime;
    unsigned long _holdDurationMs;
};

#endif
