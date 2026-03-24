#ifndef WEIGHT_SENSOR_H
#define WEIGHT_SENSOR_H

#include "../../Interfaces/HardwareSensor.h"

/**
 * A basic sensor that passes raw integer values from an IRawSource 
 * directly to the registry as floats with EMA filtering and hysteresis.
 */
class WeightSensor : public HardwareSensor {
public:
    WeightSensor(IRawSource* source, float alpha = 1.0f, float hysteresisThreshold = 0.0f) 
        : HardwareSensor(source, alpha, hysteresisThreshold) {} // Configurable filtering

    float getReading() override {
        if (!_source) return 0.0f;
        
        RawReading deviceRaw = _source->read();
        float newValue = (float)deviceRaw.value;

        if (!_initialized) {
            _currentFilteredValue = newValue;
            _lastDisplayedValue = newValue;
            _initialized = true;
        } else {
            // Adaptive Alpha: Fast response for large changes, heavy smoothing for small ones
            float diff = std::abs(newValue - _currentFilteredValue);
            float currentAlpha = (diff > 500.0f) ? 0.8f : _alpha; 
            
            _currentFilteredValue = (currentAlpha * newValue) + ((1.0f - currentAlpha) * _currentFilteredValue);
        }

        return getStableDisplayValue(); 
    }
};

#endif
