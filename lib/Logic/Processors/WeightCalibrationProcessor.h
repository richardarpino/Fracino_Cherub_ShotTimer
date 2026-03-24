#ifndef WEIGHT_CALIBRATION_PROCESSOR_H
#define WEIGHT_CALIBRATION_PROCESSOR_H

#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "../Interfaces/ITagProcessor.h"

/**
 * Reactive Processor that converts raw HX711 counts into calibrated grams.
 * Logic: Weight = (Raw - ZeroOffset) * ScaleFactor
 */
class WeightCalibrationProcessor : public ITagProcessor {
public:
    WeightCalibrationProcessor(ISensorRegistry* registry, float offset, float scale) 
        : _registry(registry), _offset(offset), _scale(scale) {}

    void update() override {
        if (!_registry) return;
        
        // Pull the latest raw count (which is now filtered by the HardwareSensor)
        Reading raw = _registry->getLatestReading(RawWeightReading::NAME);
        
        // Apply calibration
        float calibrated = (raw.value - _offset) * _scale;
        
        // Publish to calibrated Weight tag
        _registry->publish<WeightReading>(calibrated, raw.isError);
    }

private:
    ISensorRegistry* _registry;
    float _offset;
    float _scale;
};

#endif
