#ifndef HARDWARE_WEIGHT_SENSOR_H
#define HARDWARE_WEIGHT_SENSOR_H

#include "../../Interfaces/HardwareSensor.h"

/**
 * Basic Hardware Weight Sensor.
 * Pure responsibility: Raw units -> Grams + Smoothing.
 */
class WeightSensor : public HardwareSensor {
public:
    WeightSensor(IRawSource* source, float gramsPerUnit = 0.001f) 
        : HardwareSensor(source, 0.1f, 0.1f), _gramsPerUnit(gramsPerUnit) {}

    // ISensor Implementation
    Reading getReading() override {
        if (_source) {
            RawReading raw = _source->read();
            float currentGrams = raw.value * _gramsPerUnit;
            updateFilter(currentGrams);
        }
        return Reading(getStableDisplayValue(), "g", "WEIGHT", 1, false);
    }


private:
    float _gramsPerUnit;
};

#endif
