#ifndef SENSOR_TRIGGER_FACTORY_H
#define SENSOR_TRIGGER_FACTORY_H

#include "../Interfaces/ITriggerFactory.h"
#include "../Interfaces/ISensorRegistry.h"
#include "../Logic/Triggers/ThresholdSwitch.h"
#include "../Interfaces/SensorTags.h"

/**
 * Factory that creates triggers based on sensor registry readings.
 */
class SensorTriggerFactory : public ITriggerFactory {
public:
    ITrigger* createTrigger(const char* triggerTag, ISensorRegistry* registry) override {
        if (strcmp(triggerTag, "PumpActive") == 0) {
            // For now, we return the internal pump switch if possible, 
            // but the goal is to create a ThresholdSwitch or similar.
            // Since ThresholdSwitch is a template, we need to know the type.
            return new ThresholdSwitch<PumpReading>(registry, 0.5f); 
        }
        return nullptr;
    }
};

#endif
