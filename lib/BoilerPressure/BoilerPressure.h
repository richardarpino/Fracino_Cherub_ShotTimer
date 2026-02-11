#ifndef BOILER_PRESSURE_H
#define BOILER_PRESSURE_H

#include <Arduino.h>
#include "../Interfaces/ISensor.h"

class BoilerPressure : public ISensor {
public:
    BoilerPressure(IRawSource* source) : _source(source), _pressure(0.0f), _lastSampleTime(0) {}


    // ISensor Implementation
    Reading getReading() override {
        unsigned long now = millis();
        
        // Only sample hardware once per millisecond to avoid redundant overhead
        if (now != _lastSampleTime) {
            if (_source) {
                RawReading raw = _source->read();
                float currentReading = calculatePressureFromRaw(raw);
                
                // EMA Smoothing (alpha = 0.1)
                _pressure = (_pressure * 0.9f) + (currentReading * 0.1f);
                if (_pressure < 0.0f) _pressure = 0.0f;
            }
            _lastSampleTime = now;
        }

        return Reading(_pressure, "BAR", "BOILER", 1, true);
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
    unsigned long _lastSampleTime;

    const float MAX_PRESSURE_BAR = 5.0f;
    const float MIN_VOLTAGE = 0.5f;
    const float MAX_VOLTAGE = 4.5f;
    const float R1 = 4700.0f;
    const float R2 = 10000.0f;
};

#endif
