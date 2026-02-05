#ifndef BOILER_PRESSURE_H
#define BOILER_PRESSURE_H

#include <Arduino.h>

class BoilerPressure {
public:
    BoilerPressure() : _pin(-1), _pressure(0.0f) {}

    void begin(int pin) {
        _pin = pin;
        pinMode(_pin, INPUT);
        // Initial reading to seed the smoothing
        _pressure = readRawPressure();
    }

    float getPressure() {
        if (_pin == -1) return 0.0f; // Not initialized

        float currentReading = readRawPressure();
        // Simple Exponential Moving Average (EMA) for smoothing
        // alpha = 0.1 means 10% new reading, 90% old reading
        _pressure = (_pressure * 0.9f) + (currentReading * 0.1f);

        // Clamp to 0 to avoid negative noise
        return (_pressure < 0.0f) ? 0.0f : _pressure;
    }

    /**
     * estimates the boiler temperature based on the saturated steam pressure
     * using the Antoine Equation for water.
     * T = B / (A - log10(P_mmHg)) - C
     * Returns Celsius.
     */
    float getEstimatedTemperature() {
        if (_pressure <= 0.05f) return 25.0f; // Ambient fallback if no pressure

        // 1. Convert Gauge Pressure (Bar) to Absolute Pressure (mmHg)
        // P_abs_bar = P_gauge + 1.01325
        float p_abs_bar = _pressure + 1.01325f;
        // P_mmHg = P_abs_bar * 750.062
        float p_mmHg = p_abs_bar * 750.062f;

        // 2. Antoine Equation constants for Water (100C - 374C)
        // A = 8.14019, B = 1810.94, C = 244.485
        const float A = 8.14019f;
        const float B = 1810.94f;
        const float C = 244.485f;

        // T = B / (A - log10(P)) - C
        float tempC = (B / (A - log10(p_mmHg))) - C;

        return tempC;
    }

private:
    int _pin;
    float _pressure;

    // Sensor Specs
    // Assuming 0.5MPa (5 Bar) sensor version for boiler application
    const float MAX_PRESSURE_BAR = 5.0f;
    const float MIN_VOLTAGE = 0.5f;
    const float MAX_VOLTAGE = 4.5f;
    
    // Voltage Divider Resistors
    // R1 = Series Resistor (from sensor)
    // R2 = Ground Resistor (to GND)
    const float R1 = 4700.0f;
    const float R2 = 10000.0f;

    float readRawPressure() {
        // Read ADC (0-4095 for 12-bit)
        int rawADC = analogRead(_pin);
        
        // Convert to Voltage at the Pin (ESP32 ADC ref is approx 3.3V)
        float pinVoltage = (rawADC / 4095.0f) * 3.3f;
        
        // Calculate actual Sensor Voltage (reversing voltage divider)
        // Vout = Vin * (R2 / (R1 + R2))  =>  Vin = Vout * ((R1 + R2) / R2)
        float sensorVoltage = pinVoltage * ((R1 + R2) / R2);

        // Convert Voltage to Pressure
        // Pressure = (V_sensor - 0.5V) * (P_max / 4.0V)
        // 4.0V is the span (4.5 - 0.5)
        float pressure = (sensorVoltage - MIN_VOLTAGE) * (MAX_PRESSURE_BAR / (MAX_VOLTAGE - MIN_VOLTAGE));
        
        return pressure;
    }

};

#endif
