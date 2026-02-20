#include "ShotTimer.h"

ShotTimer::ShotTimer(ISwitch* pumpSwitch, float minShotDuration) 
    : _pump(pumpSwitch), _startTime(0), _state(TIMER_READY), _finalTime(0.0), _lastValidDuration(0.0), _minShotDuration(minShotDuration) {}

Reading ShotTimer::getReading() {
    if (_pump) {
        _pump->update();

        if (_pump->justStarted()) {
            _state = TIMER_RUNNING;
            _startTime = millis();
            _finalTime = 0.0;
        }

        if (_pump->justStopped()) {
            _state = TIMER_FINISHED;
            float duration = (millis() - _startTime) / 1000.0;
            if (duration >= _minShotDuration) {
                _finalTime = duration;
                _lastValidDuration = duration;
            } else {
                _finalTime = _lastValidDuration;
            }
        }
    }

    float time = (_state == TIMER_RUNNING) ? (millis() - _startTime) / 1000.0 : _finalTime;
    String status = (_state == TIMER_RUNNING) ? "SHOT RUNNING" : "READY";
    return Reading(time, "SECS", status, 1, false);
}
