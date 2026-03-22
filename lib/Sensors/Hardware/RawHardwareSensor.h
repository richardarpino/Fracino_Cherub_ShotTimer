#ifndef RAW_HARDWARE_SENSOR_H
#define RAW_HARDWARE_SENSOR_H

#include "../../Interfaces/HardwareSensor.h"

/**
 * A basic sensor that passes raw integer values from an IRawSource 
 * directly to the registry as floats. Useful for calibration and diagnostics.
 */
class RawHardwareSensor : public HardwareSensor {
public:
    RawHardwareSensor(IRawSource* source) 
        : HardwareSensor(source, 1.0f, 0.0f) {} // No filtering by default

    float getReading() override {
        if (!_source) return 0.0f;
        RawReading raw = _source->read();
        updateFilter((float)raw.value);
        return _currentFilteredValue; // Return current raw value
    }
};

#endif
