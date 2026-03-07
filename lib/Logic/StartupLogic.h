#ifndef STARTUP_LOGIC_H
#define STARTUP_LOGIC_H

#include "../Interfaces/SensorTypes.h"
#include "../Interfaces/ISwitch.h"
#include "../Interfaces/IBlocker.h"
#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "../Sensors/Registry/RegistrySwitch.h"

class ISwitchProvider;

class StartupLogic : public IBlocker {
public:
    StartupLogic(ISwitchProvider* provider, ISensorRegistry* registry, unsigned long holdDurationMs = 3000);
    
    void update() override;
    
    // IBlocker implementation
    StatusMessage getStatus() const override;
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

    RegistrySwitch<WiFiStatus> _wifiSwitch;
    RegistrySwitch<OTAStatus> _otaSwitch;
    RegistrySwitch<WarmingUpStatus> _warmingUpSwitch;
    
    IBlocker* _wifi;
    IBlocker* _warmingUp;
    ISwitchProvider* _provider;
    ISensorRegistry* _registry;
    State _state;
    bool _isActive;
    bool _justStarted;
    bool _justStopped;
    bool _lastActive;
    float _startTimeSecs;
    float _holdDurationSecs;
};

#endif
