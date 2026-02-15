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
        
        // 64-sample oversampling to reduce noise and increase bit-depth precision
        long sum = 0;
        for(int i = 0; i < 64; i++) {
            sum += analogRead(_pin);
        }
        return RawReading((int)(sum / 64), millis());
    }

private:
    int _pin;
};

#endif
