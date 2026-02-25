#ifndef WARMING_UP_BLOCKER_H
#define WARMING_UP_BLOCKER_H

#include "../Interfaces/IBlocker.h"
#include "../Interfaces/ISensor.h"

#include <vector>

class WarmingUpBlocker : public IBlocker {
public:
    WarmingUpBlocker(ISensor* pressureSensor, unsigned long timeoutMs = 600000);
    // IBlocker Implementation
    BlockerStatus getStatus() const override;

    void update() override;

    // ISwitch Implementation
    bool isActive() const override;
    bool justStarted() const override;
    bool justStopped() const override;

private:
    ISensor* _pressureSensor;
    float _lastPressure;
    unsigned long _startTime;
    unsigned long _timeoutMs;
    bool _isFinished;
    bool _wasFinished;

    // Dimensional History
    std::vector<std::vector<float>> _moves;
    float _lastRoundedReading;

    const int TARGET_CYCLES = 3;
    float getProgress() const;
    void processHistory(float pressure);
};

#endif
