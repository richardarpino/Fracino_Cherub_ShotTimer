#ifndef TARED_WEIGHT_PROCESSOR_H
#define TARED_WEIGHT_PROCESSOR_H

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "Registry/RegistrySwitch.h"

/**
 * Reactive Processor that handles Taring.
 * SRP: Reads raw weight, applies offset based on Pump trigger, and publishes results.
 */
class TaredWeightProcessor {
public:
    TaredWeightProcessor(ISensorRegistry* registry) 
        : _registry(registry), _pump(registry), _offset(0.0f) {}
    
    void update() {
        if (!_registry) return;

        _pump.update();
        
        // Pull the raw weight from the registry
        Reading weight = _registry->getLatest<WeightReading>();

        // Reactive Tare: If pump just started, capture current weight as offset
        if (_pump.justStarted()) {
            _offset = weight.value;
        }

        // Calculate and publish the tared result
        float taredValue = weight.value - _offset;
        _registry->publish<TaredWeightReading>(taredValue, weight.isError);
    }

    void tare() {
        _offset = _registry->getLatest<WeightReading>().value;
    }

private:
    ISensorRegistry* _registry;
    RegistrySwitch<PumpReading> _pump;
    float _offset;
};

#endif
