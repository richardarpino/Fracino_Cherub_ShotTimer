#ifndef ISENSOR_H
#define ISENSOR_H

#include <Arduino.h>

/**
 * Raw hardware data (e.g. ADC reading, digital state)
 */
struct RawReading {
    int value;
    unsigned long timestamp;

    RawReading(int v = 0, unsigned long t = 0) : value(v), timestamp(t) {}
};

/**
 * Processed domain measurement (e.g. 1.2 Bar, 120 C)
 */
struct Reading {
    float value;
    String unit;
    String label;
    int precision;
    bool isValid;

    Reading(float v = 0.0f, String u = "", String l = "", int p = 1, bool valid = false) 
        : value(v), unit(u), label(l), precision(p), isValid(valid) {}
};

/**
 * Interface for a hardware source (ADC, GPIO, etc.)
 * This allows mocking hardware for unit tests.
 */
class IRawSource {
public:
    virtual ~IRawSource() {}
    virtual RawReading read() = 0;
};

/**
 * Interface for a component that processes raw values into domain readings.
 */
class ISensor {
public:
    virtual ~ISensor() {}
    virtual Reading getReading() = 0;
};

#endif
