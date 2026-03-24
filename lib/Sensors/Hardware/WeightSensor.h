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
        : HardwareSensor(source, alpha, hysteresisThreshold), _lastProcessedTimestamp(0) {} 

    float getReading() override {
        if (!_source) return 0.0f;
        
        RawReading deviceRaw = _source->read();
        
        // ONLY update the filter if we have a NEW sample from the hardware (approx 10Hz/80Hz)
        // This prevents the EMA from being "diluted" by the 60Hz loop speed.
        if (deviceRaw.timestamp != _lastProcessedTimestamp) {
            float newValue = (float)deviceRaw.value;

            if (!_initialized) {
                _currentFilteredValue = newValue;
                _lastDisplayedValue = newValue;
                _initialized = true;
            } else {
                // Adaptive Alpha: Fast response (0.8) for changes > 100 counts (~0.18g)
                // Precision response (0.5) for small adjustments to ensure we punch through hysteresis.
                float diff = std::abs(newValue - _currentFilteredValue);
                float currentAlpha = (diff > 100.0f) ? 0.8f : 0.5f; 
                
                _currentFilteredValue = (currentAlpha * newValue) + ((1.0f - currentAlpha) * _currentFilteredValue);
            }
            _lastProcessedTimestamp = deviceRaw.timestamp;
        }

        return getStableWeight(); 
    }
 
protected:
    /**
     * Specialized stability for weight: Snaps to absolute zero if the 
     * filtered value is within the noise floor (hysteresis) of zero.
     */
    float getStableWeight() {
        float val = HardwareSensor::getStableDisplayValue();
        if (std::abs(val) < _hysteresisThreshold) {
            return 0.0f;
        }
        return val;
    }

private:
    unsigned long _lastProcessedTimestamp;
};

#endif
