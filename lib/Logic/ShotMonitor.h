#ifndef SHOT_MONITOR_H
#define SHOT_MONITOR_H

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "../Sensors/Registry/RegistrySwitch.h"
#include "ManualPumpTimer.h"

/**
 * Monitors pump state to coordinate shot timing.
 * Implements the "Last Good Shot" rule (Issue #10).
 */
class ShotMonitor {
public:
    ShotMonitor(ManualPumpTimer* timer, ISensorRegistry* registry);

    void update();

private:
    RegistrySwitch<PumpTag> _pump;
    ManualPumpTimer* _timer;
    ISensorRegistry* _registry;
    float _lastValidDuration;
};

#endif
