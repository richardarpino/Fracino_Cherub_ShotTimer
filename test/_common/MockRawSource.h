#ifndef MOCK_RAW_SOURCE_H
#define MOCK_RAW_SOURCE_H

#include "../lib/Interfaces/ISensor.h"

/**
 * Mock implementation of IRawSource for unit testing.
 */
class MockRawSource : public IRawSource {
public:
    MockRawSource() : _value(0) {}

    void setRawValue(int v) {
        _value = v;
    }

    RawReading read() override {
        return RawReading(_value, millis());
    }

private:
    int _value;
};

#endif
