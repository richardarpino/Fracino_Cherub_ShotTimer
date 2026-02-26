#ifndef VIRTUAL_TARED_WEIGHT_H
#define VIRTUAL_TARED_WEIGHT_H

#include "../Interfaces/SensorTypes.h"
#include "../Interfaces/HardwareSensor.h"

/**
 * Logical decorator to add Taring functionality.
 * Note: No longer an ISensor. Orchestrators publish its results to the Registry.
 */
class TaredWeight {
public:
    TaredWeight(HardwareSensor* base) 
        : _base(base), _offset(0.0f) {}
    
    Reading getReading() {
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
    HardwareSensor* _base;
    float _offset;
};

#endif
