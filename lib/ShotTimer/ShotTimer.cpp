#include "ShotTimer.h"

ShotTimer::ShotTimer(IRawSource* pumpSource, unsigned long debounceMs, float minShotDuration) 
    : _pumpSource(pumpSource), _debounceTime(debounceMs), _startTime(0), _lastActiveTime(0), _lastSampleTime(0), _state(TIMER_READY), _finalTime(0.0), _lastValidDuration(0.0), _minShotDuration(minShotDuration) {}

Reading ShotTimer::getReading() {
    unsigned long now = millis();

    // Only sample/update once per millisecond to avoid redundant hardware polls
    if (now != _lastSampleTime) {
        if (_pumpSource) {
            RawReading raw = _pumpSource->read();
            bool isPumpActive = (raw.value == LOW);
            unsigned long currentTime = raw.timestamp;

            if (isPumpActive) {
                _lastActiveTime = currentTime;
                if (_state != TIMER_RUNNING) {
                    _state = TIMER_RUNNING;
                    _startTime = currentTime;
                    _finalTime = 0.0;
                }
            }

            if (_state == TIMER_RUNNING) {
                if (currentTime - _lastActiveTime > _debounceTime) {
                    _state = TIMER_FINISHED;
                    float duration = (_lastActiveTime - _startTime) / 1000.0;
                    if (duration >= _minShotDuration) {
                        _finalTime = duration;
                        _lastValidDuration = duration;
                    } else {
                        _finalTime = _lastValidDuration;
                    }
                }
            }
        }
        _lastSampleTime = now;
    }

    float time = (_state == TIMER_RUNNING) ? (millis() - _startTime) / 1000.0 : _finalTime;
    String status = (_state == TIMER_RUNNING) ? "SHOT RUNNING" : "READY";
    return Reading(time, "SECS", status, 1, false);
}
