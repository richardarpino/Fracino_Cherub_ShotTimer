#include "BoilerTemperatureProcessor.h"
#include <cmath>

BoilerTemperatureProcessor::BoilerTemperatureProcessor(ISensorRegistry* registry)
    : _registry(registry), _currentFilteredValue(0.0f), 
      _lastDisplayedValue(0.0f), _initialized(false) {}

void BoilerTemperatureProcessor::update() {
    if (!_registry) return;
    
    Reading pressure = _registry->getLatest<BoilerPressureReading>();
    if (pressure.isError) return;

    float currentTemp = calculateTemperature(pressure.value);
    updateFilter(currentTemp);
    
    _registry->publish<BoilerTempReading>(getStableDisplayValue());
}

void BoilerTemperatureProcessor::updateFilter(float newValue) {
    float alpha = 0.05f;
    if (!_initialized) {
        _currentFilteredValue = newValue;
        _lastDisplayedValue = newValue;
        _initialized = true;
    } else {
        _currentFilteredValue = (alpha * newValue) + ((1.0f - alpha) * _currentFilteredValue);
    }
}

float BoilerTemperatureProcessor::getStableDisplayValue() {
    float threshold = 0.5f;
    float diff = _currentFilteredValue - _lastDisplayedValue;
    if (std::abs(diff) > threshold) {
        _lastDisplayedValue = _currentFilteredValue;
    }
    return _lastDisplayedValue;
}

float BoilerTemperatureProcessor::calculateTemperature(float pressureBar) {
    if (pressureBar <= 0.05f) return 25.0f;
    float p_abs_bar = pressureBar + 1.01325f;
    float p_mmHg = p_abs_bar * 750.062f;
    const float A = 8.14019f;
    const float B = 1810.94f;
    const float C = 244.485f;
    // T = (B / (A - log10(P))) - C
    return (B / (A - std::log10(p_mmHg))) - C;
}
