#ifndef WARMING_UP_BLOCKER_H
#define WARMING_UP_BLOCKER_H

#include "../Interfaces/IBlocker.h"
#include "../Interfaces/HardwareSensor.h"
#include "../Logic/Triggers/ThresholdSwitch.h"

#include <vector>

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"

class WarmingUpBlocker : public IBlocker {
public:
    WarmingUpBlocker(ISensorRegistry* registry, unsigned long timeoutMs = 600000);
    virtual ~WarmingUpBlocker() = default;
    // IBlocker Implementation
    StatusMessage getStatus() const override;

    void update() override;

    // ISwitch Implementation
    bool isActive() const override;
    bool justStarted() const override;
    bool justStopped() const override;

    ISensorRegistry* _registry;
    bool _isFinished;
    bool _wasFinished;
};

#endif
