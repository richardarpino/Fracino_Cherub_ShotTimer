#ifndef HARDWARE_SWITCH_H
#define HARDWARE_SWITCH_H

#include "../../Interfaces/ISwitch.h"
#include "../../Interfaces/SensorTypes.h"

class HardwareSwitch : public ISwitch {
public:
    HardwareSwitch(IRawSource* source, bool activeLow = true) 
        : _source(source), _activeLow(activeLow), _currentState(false), _lastState(false), _lastUpdateMillis(0) {}
    
    void update() override {
        unsigned long now = millis();
        if (now == _lastUpdateMillis) return;
        _lastUpdateMillis = now;

        _lastState = _currentState;
        if (_source) {
            bool raw = _source->read().value == (_activeLow ? LOW : HIGH);
            _currentState = raw;
        }
    }
    
    bool isActive() const override { return _currentState; }
    bool justStarted() const override { return _currentState && !_lastState; }
    bool justStopped() const override { return !_currentState && _lastState; }

private:
    IRawSource* _source;
    bool _activeLow;
    bool _currentState;
    bool _lastState;
    unsigned long _lastUpdateMillis;
};

#endif
