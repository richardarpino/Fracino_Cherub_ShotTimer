#ifndef HARDWARE_SENSOR_H
#define HARDWARE_SENSOR_H

#include "SensorTypes.h"

/**
 * Mandatory base class for all physical hardware sensors.
 * Consolidates EMA filtering and hysteresis logic.
 * Enforces a pattern where sensors are passive observers of a physical signal.
 */
class HardwareSensor {
public:
    HardwareSensor(IRawSource* source, float alpha = 0.1f, float hysteresisThreshold = 0.05f) 
        : _source(source), _alpha(alpha), _hysteresisThreshold(hysteresisThreshold), 
          _currentFilteredValue(0.0f), _lastDisplayedValue(0.0f), 
          _initialized(false) {}

    virtual ~HardwareSensor() = default;

    /**
     * Physical sensors must provide metadata and their current processed reading.
     */
    virtual Reading getReading() = 0;
    virtual SensorMetadata getMetadata() = 0;

    /**
     * Returns the high-precision filtered value before hysteresis.
     * Useful for logic components that derive data from this sensor.
     */
    float getFilteredValue() const { return _currentFilteredValue; }

protected:
    /**
     * Updates the internal filter with a new raw measurement.
     * Concrete implementations should call this in their getReading() or similar.
     */
    void updateFilter(float newValue) {
        if (!_initialized) {
            _currentFilteredValue = newValue;
            _lastDisplayedValue = newValue;
            _initialized = true;
        } else {
            // EMA: new = (alpha * current) + ((1 - alpha) * previous)
            _currentFilteredValue = (_alpha * newValue) + ((1.0f - _alpha) * _currentFilteredValue);
        }
    }

    /**
     * Applies hysteresis to the filtered value to prevent display flickering.
     */
    float getStableDisplayValue() {
        float diff = _currentFilteredValue - _lastDisplayedValue;
        if (std::abs(diff) > _hysteresisThreshold) {
            _lastDisplayedValue = _currentFilteredValue;
        }
        return _lastDisplayedValue;
    }

    IRawSource* _source;
    float _alpha;
    float _hysteresisThreshold;
    float _currentFilteredValue;
    float _lastDisplayedValue;
    bool _initialized;
};

#endif
