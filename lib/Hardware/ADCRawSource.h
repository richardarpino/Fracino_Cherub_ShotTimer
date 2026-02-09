#ifndef ADC_RAW_SOURCE_H
#define ADC_RAW_SOURCE_H

#include "../Interfaces/ISensor.h"

/**
 * Concrete implementation of IRawSource for ESP32/Arduino ADC.
 */
class ADCRawSource : public IRawSource {
public:
    ADCRawSource(int pin) : _pin(pin) {
        if (_pin != -1) {
            pinMode(_pin, INPUT);
        }
    }

    RawReading read() override {
        if (_pin == -1) return RawReading(0, millis());
        return RawReading(analogRead(_pin), millis());
    }

private:
    int _pin;
};

#endif
