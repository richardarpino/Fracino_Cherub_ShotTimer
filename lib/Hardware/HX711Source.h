#ifndef HX711_SOURCE_H
#define HX711_SOURCE_H

#include <Arduino.h>
#include "SensorTypes.h"

/**
 * Non-blocking hardware source for the HX711 Load Cell Amplifier.
 * 
 * Logic:
 * 1. HX711 signals data ready by pulling DOUT LOW.
 * 2. If DOUT is HIGH, we return the last cached value immediately (non-blocking).
 * 3. If DOUT is LOW, we pulse SCK 24 times to shift out the data, plus a 25th 
 *    pulse to set the gain for the next reading.
 */
class HX711Source : public IRawSource {
public:
    HX711Source(int dout, int sck) : _dout(dout), _sck(sck), _lastValue(0) {
        pinMode(_dout, INPUT);
        pinMode(_sck, OUTPUT);
        digitalWrite(_sck, LOW);
    }

    RawReading read() override {
        // Data is ready when DOUT goes LOW
        if (digitalRead(_dout) == HIGH) {
            return RawReading(_lastValue, millis());
        }

        // Read 24 bits
        long value = 0;
        for (int i = 0; i < 24; i++) {
            digitalWrite(_sck, HIGH);
            // On ESP32 at 80MHz+, we might need a tiny delay, 
            // but for a 1.14" display clone at standard speeds, 
            // the digitalWrite overhead is often enough for the HX711 (T_2 > 0.1us).
            value = (value << 1) | digitalRead(_dout);
            digitalWrite(_sck, LOW);
        }

        // 25th pulse: Channel A, Gain 128
        digitalWrite(_sck, HIGH);
        digitalWrite(_sck, LOW);

        // HX711 data is 24-bit 2's complement (signed)
        // We extend the sign bit to 32 bits.
        if (value & 0x800000) {
            value |= 0xFF000000;
        }

        _lastValue = (int)value;
        return RawReading(_lastValue, millis());
    }

private:
    int _dout;
    int _sck;
    int _lastValue;
};

#endif
