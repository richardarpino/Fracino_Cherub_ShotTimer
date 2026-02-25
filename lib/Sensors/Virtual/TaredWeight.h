#ifndef VIRTUAL_TARED_WEIGHT_H
#define VIRTUAL_TARED_WEIGHT_H

#include "../../Interfaces/ISensor.h"

/**
 * Virtual Sensor: Decorator to add Taring functionality.
 * Wraps any ISensor and provides a zeroed reference.
 * 
 * Behavior (when to tare) is coordinated by external logic modules.
 */
class TaredWeight : public ISensor {
public:
    TaredWeight(ISensor* base) 
        : _base(base), _offset(0.0f) {}
    
    Reading getReading() override {
        if (!_base) return Reading();
        
        Reading r = _base->getReading();
        r.value -= _offset;
        return r;
    }

    SensorMetadata getMetadata() override {
        if (_base) {
            SensorMetadata baseMeta = _base->getMetadata();
            return SensorMetadata(
                Reading(-2000.0f, "g", "WEIGHT", 1, false), 
                baseMeta.high, 
                baseMeta.init, 
                baseMeta.error
            );
        }
        return SensorMetadata(Reading(), Reading(), Reading(), Reading());
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
