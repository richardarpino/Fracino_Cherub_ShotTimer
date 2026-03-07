#ifndef SYSTEM_TIME_SENSOR_H
#define SYSTEM_TIME_SENSOR_H

#include <Arduino.h>
#include "../../Interfaces/HardwareSensor.h"

/**
 * A simple bridge sensor that provides system uptime in seconds.
 * Does not require filtering or hysteresis as it's a monotonic counter.
 */
class SystemTimeSensor : public HardwareSensor {
public:
    SystemTimeSensor() : HardwareSensor(nullptr, 1.0f, 0.0f) {}

    float getReading() override {
        return millis() / 1000.0f;
    }
};

#endif
