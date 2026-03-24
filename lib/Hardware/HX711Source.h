#ifndef HX711_SOURCE_H
#define HX711_SOURCE_H

#include <Arduino.h>
#include "SensorTypes.h"

/**
 * Non-blocking hardware source for the HX711 Load Cell Amplifier.
 */
class HX711Source : public IRawSource {
public:
    HX711Source(int dout, int sck) : _dout(dout), _sck(sck), _lastValue(0), _lastReadTime(0) {
        pinMode(_dout, INPUT);
        pinMode(_sck, OUTPUT);
        digitalWrite(_sck, LOW);
    }

    RawReading read() override {
        // Data is ready when DOUT goes LOW
        if (digitalRead(_dout) == HIGH) {
            return RawReading(_lastValue, _lastReadTime);
        }

        // Read 24 bits
        long value = 0;
        noInterrupts(); // Protect timing-sensitive bit-banging
        for (int i = 0; i < 24; i++) {
            digitalWrite(_sck, HIGH);
            delayMicroseconds(1); // Ensure HX711 output settling
            value = (value << 1) | digitalRead(_dout);
            digitalWrite(_sck, LOW);
            delayMicroseconds(1); // SCK low time
        }
 
        // 25th pulse: Channel A, Gain 128
        digitalWrite(_sck, HIGH);
        delayMicroseconds(1);
        digitalWrite(_sck, LOW);
        interrupts();

        // 24-bit sign extension
        if (value & 0x800000) {
            value |= 0xFF000000;
        }

        _lastValue = (int)value;
        _lastReadTime = millis();
        
        return RawReading(_lastValue, _lastReadTime);
    }

private:
    int _dout;
    int _sck;
    int _lastValue;
    unsigned long _lastReadTime;
};

#endif
