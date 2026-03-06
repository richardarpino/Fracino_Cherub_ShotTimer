#ifndef WARMING_UP_PROCESSOR_H
#define WARMING_UP_PROCESSOR_H

#include "../../Interfaces/ITagProcessor.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"

/**
 * Calculates WarmingUpStatus based on HeatingCycleReading and BoilerPressureReading.
 * Manages timeout and progress tracking.
 */
class WarmingUpProcessor : public ITagProcessor {
public:
    WarmingUpProcessor(ISensorRegistry* registry, unsigned long timeoutMs = 600000);
    
    void update() override;

private:
    ISensorRegistry* _registry;
    unsigned long _startTime;
    unsigned long _timeoutMs;
    bool _isFinished;
    
    mutable char _statusBuffer[64];
    const int TARGET_CYCLES = 3;

    float getProgress() const;
};

#endif
