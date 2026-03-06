#ifndef BOILER_SAFETY_PROCESSOR_H
#define BOILER_SAFETY_PROCESSOR_H

#include "../../Interfaces/ITagProcessor.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"
#include "../Triggers/ThresholdSwitch.h"

/**
 * Monitors Boiler Pressure for dangerous levels.
 * Publishes alert status to BoilerSafetyStatus.
 */
class BoilerSafetyProcessor : public ITagProcessor {
public:
    BoilerSafetyProcessor(ISensorRegistry* registry);
    
    void update() override;

private:
    ISensorRegistry* _registry;
    ThresholdSwitch<BoilerPressureReading> _trigger;
};

#endif
