#ifndef TARED_WEIGHT_PROCESSOR_H
#define TARED_WEIGHT_PROCESSOR_H

#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "Registry/RegistrySwitch.h"

#include "../Interfaces/ITagProcessor.h"

/**
 * Reactive Processor that handles Taring.
 * SRP: Reads raw weight, applies offset based on Pump trigger, and publishes results.
 */
class TaredWeightProcessor : public ITagProcessor {
public:
    TaredWeightProcessor(ISensorRegistry* registry) 
        : _registry(registry), _pump(registry), _offset(0.0f), _isLocked(false) {}
    
    void update() {
        if (!_registry) return;

        _pump.update();
        
        // Reactive Tare: If pump just started, capture current weight as offset and unlock
        if (_pump.justStarted()) {
            _offset = _registry->getLatest<WeightReading>().value;
            _isLocked = false;
        }

        // Lock trigger: If pump just stopped, freeze the current reading
        if (_pump.justStopped()) {
            _isLocked = true;
        }

        // If locked, we do not update the published value from the live registry
        if (!_isLocked) {
           Reading weight = _registry->getLatest<WeightReading>();
           float taredValue = weight.value - _offset;
           _registry->publish<TaredWeightReading>(taredValue, weight.isError);
        }
    }

    void tare() {
        _offset = _registry->getLatest<WeightReading>().value;
        _isLocked = false;
    }

private:
    ISensorRegistry* _registry;
    RegistrySwitch<PumpReading> _pump;
    float _offset;
    bool _isLocked;
};

#endif
