#ifndef VIRTUAL_BOILER_TEMPERATURE_H
#define VIRTUAL_BOILER_TEMPERATURE_H

#include "../../Interfaces/ISensor.h"
#include "../Hardware/BoilerPressure.h"

/**
 * Virtual Sensor that derives Temperature from a Pressure Sensor reading.
 */
class BoilerTemperature : public ISensor {
public:
    BoilerTemperature(BoilerPressure* pressureSensor) 
        : _pressureSensor(pressureSensor) {}


    Reading getReading() override {
        if (!_pressureSensor) return Reading(25.0f, "C", "TEMP", 0, true);
        
        float pressureBar = _pressureSensor->getReading().value;
        float temp = calculateTemperature(pressureBar);
        
        return Reading(temp, "C", "TEMP", 0, false);
    }

    SensorMetadata getMetadata() override {
        return SensorMetadata(
            Reading(25.0f, "C", "TEMP", 0, false),
            Reading(125.0f, "C", "TEMP", 0, false),
            Reading(25.0f, "C", "TEMP", 0, false),
            Reading(0.0f, "C", "TEMP ERR", 0, true)
        );
    }

private:
    BoilerPressure* _pressureSensor;

    float calculateTemperature(float pressureBar) {
        if (pressureBar <= 0.05f) return 25.0f; // Ambient fallback

        // 1. Convert Gauge Pressure (Bar) to Absolute Pressure (mmHg)
        float p_abs_bar = pressureBar + 1.01325f;
        float p_mmHg = p_abs_bar * 750.062f;

        // 2. Antoine Equation constants for Water (100C - 374C)
        const float A = 8.14019f;
        const float B = 1810.94f;
        const float C = 244.485f;

        // T = B / (A - log10(P)) - C
        return (B / (A - log10(p_mmHg))) - C;
    }
};

#endif
