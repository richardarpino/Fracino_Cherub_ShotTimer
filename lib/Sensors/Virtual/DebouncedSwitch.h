#ifndef DEBOUNCED_SWITCH_H
#define DEBOUNCED_SWITCH_H

#include "../../Interfaces/ISwitch.h"

class DebouncedSwitch : public ISwitch {
public:
    DebouncedSwitch(ISwitch* base, unsigned long delayMs) 
        : _base(base), _delayMs(delayMs), _lastActiveTime(0), _isActive(false), _lastIsActive(false), _lastUpdateMillis(0) {}
    
    void update() override {
        unsigned long now = millis();
        if (now == _lastUpdateMillis) return;
        _lastUpdateMillis = now;

        _lastIsActive = _isActive;
        if (_base) {
            _base->update();
            bool baseActive = _base->isActive();
            
            if (baseActive) {
                _isActive = true;
                _lastActiveTime = now;
            } else {
                if (_isActive && (now - _lastActiveTime >= _delayMs)) {
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
    unsigned long _lastUpdateMillis;
};

#endif
