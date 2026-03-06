#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <Arduino.h>
#include <vector>
#include <initializer_list>

/**
 * Utility for defining lists of tags at compile-time.
 */
template<typename... T>
struct TagList {};

/**
 * Raw hardware data (e.g. ADC reading, digital state)
 */
struct RawReading {
    int value;
    unsigned long timestamp;

    RawReading(int v = 0, unsigned long t = 0) : value(v), timestamp(t) {}
};

/**
 * Physical categories for sensor data.
 * Used for unit-safe comparisons and triggers.
 */
enum class PhysicalQuantity {
    NONE,
    TEMPERATURE,
    PRESSURE,
    WEIGHT,
    TIME,
    POWER,        // 0-100% or 0.0-1.0
    SIGNAL_LEVEL, // dBm
    BOOLEAN,      // Pump, Buttons, etc.
    COUNTER       // Heating cycles, etc.
};

/**
 * Trigger direction for ThresholdSwitch.
 */
enum class ThresholdMode {
    ABOVE,
    BELOW
};

/**
 * Processed domain measurement (e.g. 1.2 Bar, 120 C)
 */
struct Reading {
    float value;
    PhysicalQuantity quantity;
    const char* unit;
    const char* label;
    int precision;
    bool isError;

    Reading(float v = 0.0f, const char* u = "", const char* l = "", int p = 1, bool error = false, PhysicalQuantity q = PhysicalQuantity::NONE) 
        : value(v), quantity(q), unit(u), label(l), precision(p), isError(error) {}
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
 * Metadata describing representative states of a sensor.
 */
struct SensorMetadata {
    Reading low;
    Reading high;
    Reading init;
    Reading error;

    SensorMetadata() = default;
    SensorMetadata(Reading l, Reading h, Reading i, Reading e)
        : low(l), high(h), init(i), error(e) {}
};

/**
 * Structured status for a process or service.
 */
struct StatusMessage {
    const char* title;    // e.g., "Warming Up..."
    const char* message;  // e.g., "Heating Cycle 2, currently 0.8bar"
    float progress;  // 0.0 - 100.0, -1.0 for indeterminate
    bool isFailed;

    StatusMessage(const char* t = "", const char* m = "", float p = -1.0f, bool f = false) 
        : title(t), message(m), progress(p), isFailed(f) {}
};

/**
 * Metadata describing representative states of a service.
 */
struct ServiceMetadata {
    StatusMessage pending;
    StatusMessage active;
    StatusMessage ready;
    StatusMessage failed;

    ServiceMetadata() = default;
    ServiceMetadata(StatusMessage p, StatusMessage a, StatusMessage r, StatusMessage f)
        : pending(p), active(a), ready(r), failed(f) {}
};

#endif
