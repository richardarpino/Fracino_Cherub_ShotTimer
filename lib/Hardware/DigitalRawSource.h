#ifndef DIGITAL_RAW_SOURCE_H
#define DIGITAL_RAW_SOURCE_H

#include "../Interfaces/SensorTypes.h"

/**
 * Concrete implementation of IRawSource for Digital Pins (e.g. Pump, Buttons).
 */
class DigitalRawSource : public IRawSource {
public:
    DigitalRawSource(int pin, int mode = INPUT_PULLUP) : _pin(pin) {
        if (_pin != -1) {
            pinMode(_pin, mode);
        }
    }

    RawReading read() override {
        if (_pin == -1) return RawReading(HIGH, millis()); // Default inactive
        return RawReading(digitalRead(_pin), millis());
    }

private:
    int _pin;
};

#endif
