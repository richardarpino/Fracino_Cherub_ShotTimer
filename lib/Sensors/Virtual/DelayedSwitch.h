#ifndef DELAYED_SWITCH_H
#define DELAYED_SWITCH_H

#include "../../Interfaces/ISwitch.h"

class DelayedSwitch : public ISwitch {
public:
    DelayedSwitch(ISwitch* base, unsigned long delayMs) 
        : _base(base), _delayMs(delayMs), _lastActiveTime(0), _isActive(false), _lastIsActive(false) {}
    
    void update() override {
        _lastIsActive = _isActive;
        if (_base) {
            _base->update();
            bool baseActive = _base->isActive();
            
            if (baseActive) {
                _isActive = true;
                _lastActiveTime = millis();
            } else {
                if (_isActive && (millis() - _lastActiveTime >= _delayMs)) {
                    _isActive = false;
                }
            }
        }
    }
    
    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _isActive && !_lastIsActive; }
    bool justStopped() const override { return !_isActive && _lastIsActive; }

private:
    ISwitch* _base;
    unsigned long _delayMs;
    unsigned long _lastActiveTime;
    bool _isActive;
    bool _lastIsActive;
};

#endif
