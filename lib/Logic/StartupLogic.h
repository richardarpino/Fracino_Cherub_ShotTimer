#ifndef STARTUP_LOGIC_H
#define STARTUP_LOGIC_H

#include "../Interfaces/SensorTypes.h"
#include "../Interfaces/ISwitch.h"
#include "../Interfaces/IBlocker.h"

class ISwitchProvider;

class StartupLogic : public IBlocker {
public:
    StartupLogic(ISwitchProvider* provider, unsigned long holdDurationMs = 3000);
    
    void update() override;
    
    // IBlocker implementation
    BlockerStatus getStatus() const override;
    bool isActive() const override;
    bool justStarted() const override;
    bool justStopped() const override;

private:
    enum class State {
        SEARCHING_WIFI,
        WIFI_STABLE,
        OTA_STARTING,
        WARMING_UP,
        READY
    };

    IBlocker* _wifi;
    IBlocker* _warmingUp;
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
