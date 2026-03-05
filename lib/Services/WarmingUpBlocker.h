#ifndef WARMING_UP_BLOCKER_H
#define WARMING_UP_BLOCKER_H

#include "../Interfaces/IBlocker.h"
#include "../Interfaces/HardwareSensor.h"

#include <vector>

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"

class WarmingUpBlocker : public IBlocker {
public:
    WarmingUpBlocker(ISensorRegistry* registry, HardwareSensor* pressureSensor, unsigned long timeoutMs = 600000);
    // IBlocker Implementation
    StatusMessage getStatus() const override;

    void update() override;

    // ISwitch Implementation
    bool isActive() const override;
    bool justStarted() const override;
    bool justStopped() const override;

    ISensorRegistry* _registry;
    HardwareSensor* _pressureSensor;
    float _lastPressure;
    unsigned long _startTime;
    unsigned long _timeoutMs;
    bool _isFinished;
    bool _wasFinished;

    // Dimensional History
    std::vector<std::vector<float>> _moves;
    float _lastRoundedReading;
    mutable char _statusBuffer[64];

    const int TARGET_CYCLES = 3;
    float getProgress() const;
    void processHistory(float pressure);
};

#endif
