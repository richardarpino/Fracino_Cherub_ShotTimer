#ifndef BOILER_PRESSURE_H
#define BOILER_PRESSURE_H

#include <Arduino.h>
#include "../Interfaces/ISensor.h"

class BoilerPressure : public ISensor {
public:
    BoilerPressure(IRawSource* source) : _source(source), _pressure(0.0f) {}

    // Required for compatibility with current main.cpp initialize flow
    void begin() {
        if (_source) {
            _pressure = calculatePressureFromRaw(_source->read());
        }
    }

    // ISensor Implementation
    void update() override {
        if (!_source) return;
        
        RawReading raw = _source->read();
        float currentReading = calculatePressureFromRaw(raw);
        
        // EMA Smoothing (alpha = 0.1)
        _pressure = (_pressure * 0.9f) + (currentReading * 0.1f);
        if (_pressure < 0.0f) _pressure = 0.0f;
    }

    Reading getReading() override {
        return Reading(_pressure, "BAR", "BOILER", true);
    }

    /**
     * Pure logic for pressure calculation - Testable!
     */
    float calculatePressureFromRaw(const RawReading& raw) {
        // Convert to Voltage at the Pin (ESP32 ADC ref is approx 3.3V)
        float pinVoltage = (raw.value / 4095.0f) * 3.3f;
        
        // Calculate actual Sensor Voltage (reversing voltage divider)
        float sensorVoltage = pinVoltage * ((R1 + R2) / R2);

        // Convert Voltage to Pressure
        float pressure = (sensorVoltage - MIN_VOLTAGE) * (MAX_PRESSURE_BAR / (MAX_VOLTAGE - MIN_VOLTAGE));
        
        return pressure;
    }

private:
    IRawSource* _source;
    float _pressure;

    const float MAX_PRESSURE_BAR = 5.0f;
    const float MIN_VOLTAGE = 0.5f;
    const float MAX_VOLTAGE = 4.5f;
    const float R1 = 4700.0f;
    const float R2 = 10000.0f;
};

#endif
