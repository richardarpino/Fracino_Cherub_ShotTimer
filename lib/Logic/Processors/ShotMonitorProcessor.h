#ifndef SHOT_MONITOR_PROCESSOR_H
#define SHOT_MONITOR_PROCESSOR_H

#include "../../Interfaces/ITagProcessor.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"
#include "../../Sensors/Registry/RegistrySwitch.h"

/**
 * Monitors pump state to coordinate shot timing and persistence.
 * Reacts to PumpReading and publishes ShotTimeReading and LastValidShotReading.
 * Folded ManualPumpTimer logic directly into this processor.
 */
class ShotMonitorProcessor : public ITagProcessor {
public:
    ShotMonitorProcessor(ISensorRegistry* registry);
    
    void update() override;

private:
    ISensorRegistry* _registry;
    RegistrySwitch<PumpReading> _pump;
    
    float _startTimeSecs;
    bool _isRunning;
    float _lastDuration;
    float _lastValidDuration;

    void startTimer();
    void stopTimer();
    float getElapsedSeconds() const;
};

#endif
