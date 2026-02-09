#include "ShotTimer.h"

ShotTimer::ShotTimer(IRawSource* pumpSource, unsigned long debounceMs, float minShotDuration) 
    : _pumpSource(pumpSource), _debounceTime(debounceMs), _startTime(0), _lastActiveTime(0), _state(TIMER_READY), _finalTime(0.0), _lastValidDuration(0.0), _minShotDuration(minShotDuration) {}

void ShotTimer::update() {
    if (!_pumpSource) return;

    RawReading raw = _pumpSource->read();
    // Active LOW logic (Input Pullup): LOW = Pump Active
    bool isPumpActive = (raw.value == LOW);
    unsigned long currentTime = raw.timestamp;

    // If pump is currently active, update the last seen active time
    if (isPumpActive) {
        _lastActiveTime = currentTime;
        
        // If we were READY or FINISHED, start a new shot
        if (_state != TIMER_RUNNING) {
            _state = TIMER_RUNNING;
            _startTime = currentTime;
            _finalTime = 0.0;
        }
    }

    // Check if we are running but the pump hasn't been active for longer than debounce time
    if (_state == TIMER_RUNNING) {
        if (currentTime - _lastActiveTime > _debounceTime) {
            _state = TIMER_FINISHED;
            // Calculate final time based on the last moment the pump was actually active
            float duration = (_lastActiveTime - _startTime) / 1000.0;
            
            if (duration >= _minShotDuration) {
                _finalTime = duration;
                _lastValidDuration = duration;
            } else {
                // Discard short shot (cleaning flush), revert to last valid time
                _finalTime = _lastValidDuration;
            }
        }
    }
}

Reading ShotTimer::getReading() {
    float time = (_state == TIMER_RUNNING) ? (millis() - _startTime) / 1000.0 : _finalTime;
    return Reading(time, "SECS", "TIMER", true);
}
