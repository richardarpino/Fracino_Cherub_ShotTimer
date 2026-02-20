#ifndef VIRTUAL_TARED_WEIGHT_H
#define VIRTUAL_TARED_WEIGHT_H

#include "../../Interfaces/ISensor.h"
#include "../../Interfaces/ISwitch.h"

/**
 * Virtual Sensor: Decorator to add Taring functionality.
 * Wraps any ISensor and provides a zeroed reference.
 */
class TaredWeight : public ISensor {
public:
    TaredWeight(ISensor* base, ISwitch* trigger = nullptr) 
        : _base(base), _trigger(trigger), _offset(0.0f) {}
    
    Reading getReading() override {
        if (!_base) return Reading();

        if (_trigger) {
            _trigger->update();
            if (_trigger->justStarted()) {
                tare();
            }
        }
        
        Reading r = _base->getReading();
        r.value -= _offset;
        return r;
    }
    
    void tare() {
        if (_base) {
            _offset = _base->getReading().value;
        }
    }

private:
    ISensor* _base;
    ISwitch* _trigger;
    float _offset;
};

#endif
