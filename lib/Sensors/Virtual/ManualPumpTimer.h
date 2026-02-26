#ifndef MANUAL_PUMP_TIMER_H
#define MANUAL_PUMP_TIMER_H

#include <Arduino.h>
#include "../../Interfaces/ISensor.h"

/**
 * A Virtual Sensor that tracks pump duration.
 * Simple SRP: It only knows how to count elapsed time between start() and stop().
 */
class ManualPumpTimer : public ISensor {
public:
    ManualPumpTimer() : _startTime(0), _isRunning(false), _lastDuration(0.0) {}

    // ISensor Implementation
    Reading getReading() override {
        float time = _isRunning ? (millis() - _startTime) / 1000.0f : _lastDuration;
        String status = _isRunning ? "RUNNING" : "READY";
        return Reading(time, "SECS", status, 1, false);
    }

    SensorMetadata getMetadata() override {
        return SensorMetadata(
            Reading(0.0f, "SECS", "READY", 1, false),
            Reading(60.0f, "SECS", "RUNNING", 1, false),
            Reading(0.0f, "SECS", "READY", 1, false),
            Reading(0.0f, "SECS", "ERR", 1, true)
        );
    }

    // Control API
    void start() {
        _isRunning = true;
        _startTime = millis();
        _lastDuration = 0.0f;
    }

    void stop() {
        if (!_isRunning) return;
        _lastDuration = (millis() - _startTime) / 1000.0f;
        _isRunning = false;
    }

private:
    unsigned long _startTime;
    bool _isRunning;
    float _lastDuration;
};

#endif
