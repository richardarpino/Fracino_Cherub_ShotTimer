#ifndef VIRTUAL_TARED_SENSOR_H
#define VIRTUAL_TARED_SENSOR_H

#include "../../Interfaces/ISensor.h"

/**
 * Virtual Sensor: Decorator to add Taring functionality.
 * Wraps any ISensor and provides a zeroed reference.
 */
class TaredSensor : public ISensor {
public:
    TaredSensor(ISensor* base) : _base(base), _offset(0.0f) {}
    
    Reading getReading() override {
        if (!_base) return Reading();
        
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
    float _offset;
};

#endif
