#ifndef MANUAL_WEIGHT_PROCESSOR_H
#define MANUAL_WEIGHT_PROCESSOR_H

#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "../Interfaces/ITagProcessor.h"

/**
 * Processor for Manual/Scale mode weighing.
 * Tares when triggered and publishes to ManualWeightReading.
 * Does NOT include the lock/freeze logic used for shots.
 */
class ManualWeightProcessor : public ITagProcessor {
public:
    ManualWeightProcessor(ISensorRegistry* registry) 
        : _registry(registry), _offset(0.0f) {}
    
    void update() override {
        if (!_registry) return;

        Reading weight = _registry->getLatest<WeightReading>();
        float taredValue = weight.value - _offset;
        _registry->publish<ManualWeightReading>(taredValue, weight.isError);
    }

    void tare() {
        if (_registry) {
            _offset = _registry->getLatest<WeightReading>().value;
        }
    }

private:
    ISensorRegistry* _registry;
    float _offset;
};

#endif
