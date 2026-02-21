#include "StartupLogic.h"

StartupLogic::StartupLogic(ISensor* wifi, unsigned long holdDurationMs) 
    : _wifi(wifi), _isComplete(false), _lastComplete(false), _startTime(0), _holdDurationMs(holdDurationMs) {}

void StartupLogic::update() {
    if (!_wifi) return;
    
    Reading r = _wifi->getReading();
    bool wifiConnected = (r.value > 0.5f);

    if (wifiConnected) {
        if (_startTime == 0) {
            _startTime = millis(); // Latch the start time
        }
        
        if (millis() - _startTime >= _holdDurationMs) {
            _isComplete = true;
        }
    } else {
        _startTime = 0; // Reset if connection lost
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
