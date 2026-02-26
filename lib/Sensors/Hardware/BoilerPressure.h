#ifndef BOILER_PRESSURE_H
#define BOILER_PRESSURE_H

#include <Arduino.h>
#include "../../Interfaces/HardwareSensor.h"

class BoilerPressure : public HardwareSensor {
public:
    BoilerPressure(IRawSource* source, float scalar = 1.0f) 
        : HardwareSensor(source, 0.05f, 0.05f), 
          _lastSampleTime(0), _scalar(scalar) {}

    // ISensor Implementation
    Reading getReading() override {
        unsigned long now = millis();
        
        // Only sample hardware once per millisecond
        if (now != _lastSampleTime) {
            if (_source) {
                RawReading raw = _source->read();
                float currentReading = calculatePressureFromRaw(raw);
                updateFilter(currentReading);
            }
            _lastSampleTime = now;
        }

        return Reading(getStableDisplayValue(), "BAR", "BOILER", 1, false);
    }

    SensorMetadata getMetadata() override {
        return SensorMetadata(
            Reading(0.0f, "BAR", "BOILER", 1, false),
            Reading(3.0f, "BAR", "BOILER", 1, false),
            Reading(0.0f, "BAR", "BOILER", 1, false),
            Reading(0.0f, "BAR", "BOILER ERR", 1, true)
        );
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
        
        float finalPressure = pressure * _scalar;
        return (finalPressure < 0.0f) ? 0.0f : finalPressure;
    }

private:
    unsigned long _lastSampleTime;
    float _scalar;

    const float MAX_PRESSURE_BAR = 5.0f;
    const float MIN_VOLTAGE = 0.5f;
    const float MAX_VOLTAGE = 4.5f;
    const float R1 = 10000.0f;
    const float R2 = 10000.0f;
};

#endif
