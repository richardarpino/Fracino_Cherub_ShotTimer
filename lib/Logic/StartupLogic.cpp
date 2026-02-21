#include "StartupLogic.h"

StartupLogic::StartupLogic(ISwitch* wifi, unsigned long holdDurationMs) 
    : _wifi(wifi), _isComplete(false), _lastComplete(false), _startTime(0), _holdDurationMs(holdDurationMs) {}

void StartupLogic::update() {
    if (!_wifi) return;
    
    if (_wifi->justStarted()) {
        _startTime = millis();
    }

    if (_wifi->isActive()) {
        if (_startTime != 0 && (millis() - _startTime >= _holdDurationMs)) {
            _isComplete = true;
        }
    } else {
        _startTime = 0;
        _isComplete = false;
    }
}

bool StartupLogic::isComplete() {
    update();
    return _isComplete;
}

bool StartupLogic::justFinished() {
    bool current = isComplete();
    bool finished = current && !_lastComplete;
    _lastComplete = current;
    return finished;
}
