#ifndef BOILER_TEMPERATURE_H
#define BOILER_TEMPERATURE_H

#include "../Interfaces/SensorTypes.h"
#include <cmath> 
#include "../Sensors/Hardware/BoilerPressure.h"

/**
 * Logical component that derives Temperature from Pressure.
 * SRP: Pure math + state smoothing.
 * Note: No longer an ISensor, Registry produces this reading via Logic Orchestrator.
 */
class BoilerTemperature {
public:
    BoilerTemperature(BoilerPressure* pressureSensor) 
        : _pressureSensor(pressureSensor), _currentFilteredValue(0.0f), 
          _lastDisplayedValue(0.0f), _initialized(false) {}

    Reading getReading() {
        if (!_pressureSensor) return Reading(25.0f, "C", "TEMP", 0, true);
        
        float pressureBar = _pressureSensor->getFilteredValue();
        float currentTemp = calculateTemperature(pressureBar);
        
        updateFilter(currentTemp);
        
        return Reading(getStableDisplayValue(), "C", "TEMP", 0, false);
    }


private:
    BoilerPressure* _pressureSensor;
    float _currentFilteredValue;
    float _lastDisplayedValue;
    bool _initialized;

    void updateFilter(float newValue) {
        float alpha = 0.05f;
        if (!_initialized) {
            _currentFilteredValue = newValue;
            _lastDisplayedValue = newValue;
            _initialized = true;
        } else {
            _currentFilteredValue = (alpha * newValue) + ((1.0f - alpha) * _currentFilteredValue);
        }
    }

    float getStableDisplayValue() {
        float threshold = 0.5f;
        float diff = _currentFilteredValue - _lastDisplayedValue;
        if (std::abs(diff) > threshold) {
            _lastDisplayedValue = _currentFilteredValue;
        }
        return _lastDisplayedValue;
    }

    float calculateTemperature(float pressureBar) {
        if (pressureBar <= 0.05f) return 25.0f;
        float p_abs_bar = pressureBar + 1.01325f;
        float p_mmHg = p_abs_bar * 750.062f;
        const float A = 8.14019f;
        const float B = 1810.94f;
        const float C = 244.485f;
        return (B / (A - log10(p_mmHg))) - C;
    }
};

#endif
