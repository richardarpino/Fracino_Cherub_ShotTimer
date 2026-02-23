#ifndef HARDWARE_WEIGHT_SENSOR_H
#define HARDWARE_WEIGHT_SENSOR_H

#include "../../Interfaces/FilteredSensor.h"

/**
 * Basic Hardware Weight Sensor.
 * Pure responsibility: Raw units -> Grams + Smoothing.
 */
class WeightSensor : public FilteredSensor {
public:
    WeightSensor(IRawSource* source, float gramsPerUnit = 0.001f) 
        : FilteredSensor(0.1f, 0.1f), _source(source), _gramsPerUnit(gramsPerUnit) {}

    // ISensor Implementation
    Reading getReading() override {
        unsigned long now = millis();
        if (_source) {
            RawReading raw = _source->read();
            float currentGrams = raw.value * _gramsPerUnit;
            updateFilter(currentGrams);
        }
        return Reading(getStableDisplayValue(), "g", "WEIGHT", 1, false);
    }

    SensorMetadata getMetadata() override {
        return SensorMetadata(
            Reading(0.0f, "g", "WEIGHT", 1, false),
            Reading(2000.0f, "g", "WEIGHT", 1, false),
            Reading(0.0f, "g", "WEIGHT", 1, false),
            Reading(0.0f, "g", "WEIGHT ERR", 1, true)
        );
    }

private:
    IRawSource* _source;
    float _gramsPerUnit;
};

#endif
