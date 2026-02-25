#ifndef VIRTUAL_BOILER_TEMPERATURE_H
#define VIRTUAL_BOILER_TEMPERATURE_H

#include "../../Interfaces/ISensor.h"
#include <cmath> // Required for log10() in native test environment
#include "../Hardware/BoilerPressure.h"

/**
 * Virtual Sensor that derives Temperature from a Pressure Sensor reading.
 * Now uses its own filtering and stability logic to avoid inherited jitter.
 */
class BoilerTemperature : public FilteredSensor {
public:
    BoilerTemperature(BoilerPressure* pressureSensor) 
        : FilteredSensor(0.05f, 0.5f), // alpha=0.05, 0.5C Hysteresis for stability
          _pressureSensor(pressureSensor) {}


    Reading getReading() override {
        if (!_pressureSensor) return Reading(25.0f, "C", "TEMP", 0, true);
        
        // Use high-precision filtered pressure before display hysteresis
        float pressureBar = _pressureSensor->getFilteredValue();
        float currentTemp = calculateTemperature(pressureBar);
        
        // Update our own filter
        updateFilter(currentTemp);
        
        return Reading(getStableDisplayValue(), "C", "TEMP", 0, false);
    }

    SensorMetadata getMetadata() override {
        return SensorMetadata(
            Reading(25.0f, "C", "TEMP", 0, false),
            Reading(150.0f, "C", "TEMP", 0, false),
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
