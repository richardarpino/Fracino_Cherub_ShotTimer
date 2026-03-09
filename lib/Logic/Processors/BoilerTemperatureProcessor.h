#ifndef BOILER_TEMPERATURE_PROCESSOR_H
#define BOILER_TEMPERATURE_PROCESSOR_H

#include "../../Interfaces/ITagProcessor.h"
#include "../../Registry/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"

/**
 * Derives Temperature from Pressure using the Antoine equation.
 * Reacts to BoilerPressureReading and publishes BoilerTempReading.
 */
class BoilerTemperatureProcessor : public ITagProcessor {
public:
    BoilerTemperatureProcessor(ISensorRegistry* registry);
    
    void update() override;

private:
    ISensorRegistry* _registry;
    float _currentFilteredValue;
    float _lastDisplayedValue;
    bool _initialized;

    void updateFilter(float newValue);
    float getStableDisplayValue();
    float calculateTemperature(float pressureBar);
};

#endif
