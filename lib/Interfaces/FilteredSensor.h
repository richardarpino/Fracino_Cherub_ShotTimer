#ifndef FILTERED_SENSOR_H
#define FILTERED_SENSOR_H

#include "ISensor.h"

/**
 * Enhanced base class for sensors that require smoothing and display stability.
 * Provides EMA (Exponential Moving Average) and Hysteresis.
 */
class FilteredSensor : public ISensor {
public:
    FilteredSensor(float alpha = 0.1f, float hysteresisThreshold = 0.05f) 
        : _alpha(alpha), _hysteresisThreshold(hysteresisThreshold), 
          _currentFilteredValue(0.0f), _lastDisplayedValue(0.0f), 
          _initialized(false) {}

    /**
     * Updates the internal filter with a new raw measurement.
     * Should be called periodically by the concrete implementation.
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
     * Returns the value that should be shown on the UI.
     */
    float getStableDisplayValue() {
        float diff = _currentFilteredValue - _lastDisplayedValue;
        
        // Only update display if the filtered value moves beyond the threshold
        if (std::abs(diff) > _hysteresisThreshold) {
            _lastDisplayedValue = _currentFilteredValue;
        }
        
        return _lastDisplayedValue;
    }

    /**
     * Returns the high-precision filtered value before hysteresis.
     * Use this for virtual sensors that need to derive data from this sensor.
     */
    float getFilteredValue() const {
        return _currentFilteredValue;
    }

protected:
    float _alpha;               // Smoothing factor (0.0 to 1.0)
    float _hysteresisThreshold; // Minimum change required to update display
    float _currentFilteredValue;
    float _lastDisplayedValue;
    bool _initialized;
};

#endif
