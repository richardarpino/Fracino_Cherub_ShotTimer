#ifndef MANUAL_PUMP_TIMER_H
#define MANUAL_PUMP_TIMER_H

#include <Arduino.h>
#include "../Interfaces/SensorTypes.h"

/**
 * A logical component that tracks pump duration.
 * Note: No longer an ISensor. Orchestrators publish its results to the Registry.
 */
class ManualPumpTimer {
public:
    ManualPumpTimer() : _startTime(0), _isRunning(false), _lastDuration(0.0) {}

    Reading getReading() {
        float time = _isRunning ? (millis() - _startTime) / 1000.0f : _lastDuration;
        String status = _isRunning ? "RUNNING" : "READY";
        return Reading(time, "SECS", status, 1, false);
    }


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
