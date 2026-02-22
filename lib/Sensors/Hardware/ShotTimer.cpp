#include "ShotTimer.h"

ShotTimer::ShotTimer(float minShotDuration) 
    : _startTime(0), _state(TIMER_READY), _finalTime(0.0), _lastValidDuration(0.0), _minShotDuration(minShotDuration) {}

Reading ShotTimer::getReading() {
    float time = (_state == TIMER_RUNNING) ? (millis() - _startTime) / 1000.0 : _finalTime;
    String status = (_state == TIMER_RUNNING) ? "SHOT RUNNING" : "READY";
    return Reading(time, "SECS", status, 1, false);
}

SensorMetadata ShotTimer::getMetadata() {
    return SensorMetadata(
        Reading(0.0f, "SECS", "READY", 1, false),
        Reading(25.5f, "SECS", "SHOT RUNNING", 1, false),
        Reading(0.0f, "SECS", "READY", 1, false),
        Reading(0.0f, "SECS", "TIMER ERR", 1, true)
    );
}

void ShotTimer::start() {
    _state = TIMER_RUNNING;
    _startTime = millis();
    _finalTime = 0.0;
}

void ShotTimer::stop() {
    if (_state != TIMER_RUNNING) return;
    
    _state = TIMER_FINISHED;
    float duration = (millis() - _startTime) / 1000.0;
    if (duration >= _minShotDuration) {
        _finalTime = duration;
        _lastValidDuration = duration;
    } else {
        _finalTime = _lastValidDuration;
    }
}
