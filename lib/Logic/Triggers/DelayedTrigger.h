#ifndef DELAYED_TRIGGER_H
#define DELAYED_TRIGGER_H

#include "../../Interfaces/ITrigger.h"
#include <Arduino.h>

/**
 * A wrapper for ITrigger that stays active for a specified delay 
 * after the wrapped trigger becomes inactive.
 */
class DelayedTrigger : public ITrigger {
public:
    DelayedTrigger(ITrigger* wrapped, uint32_t delayMs) 
        : _wrapped(wrapped), _delayMs(delayMs), _lastActiveTime(0), _wasActive(false), _hasBeenActive(false) {}

    void update() override {
        if (!_wrapped) return;
        _wrapped->update();

        bool currentActive = _wrapped->isActive();
        
        if (currentActive) {
            _lastActiveTime = millis();
            _hasBeenActive = true;
        }
        
        _wasActive = currentActive;
    }

    bool isActive() const override {
        if (!_wrapped) return false;
        
        // Active if the inner trigger is active OR if we are within the delay period (if it has been active before)
        if (_wrapped->isActive()) return true;
        if (!_hasBeenActive) return false;
        
        return (millis() - _lastActiveTime < _delayMs);
    }

private:
    ITrigger* _wrapped;
    uint32_t _delayMs;
    uint32_t _lastActiveTime;
    bool _wasActive;
    bool _hasBeenActive;
};

#endif
