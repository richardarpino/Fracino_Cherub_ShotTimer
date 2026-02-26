#ifndef BOILER_MONITOR_H
#define BOILER_MONITOR_H

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "BoilerTemperature.h"

/**
 * Monitors boiler temperature to coordinate its publication to the Registry.
 * SRP: Focused strictly on Boiler data flow.
 */
class BoilerMonitor {
public:
    BoilerMonitor(BoilerTemperature* temp, ISensorRegistry* registry)
        : _temp(temp), _registry(registry) {}

    void update() {
        if (_temp && _registry) {
            _registry->publish<BoilerTempTag>(_temp->getReading());
        }
    }

private:
    BoilerTemperature* _temp;
    ISensorRegistry* _registry;
};

#endif
