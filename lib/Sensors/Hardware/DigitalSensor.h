#ifndef DIGITAL_SENSOR_H
#define DIGITAL_SENSOR_H

#include "../../Interfaces/HardwareSensor.h"

/**
 * Concrete Hardware Sensor for Digital Inputs.
 * Bridges IRawSource (Pins) to the Registry with optional inversion.
 */
class DigitalSensor : public HardwareSensor {
public:
    DigitalSensor(IRawSource* source, bool activeLow = true, unsigned long debounceMs = 50) 
        : HardwareSensor(source, 1.0f, 0.5f), 
          _activeLow(activeLow), 
          _debounceMs(debounceMs),
          _lastRawActive(false),
          _lastDebounceTime(0),
          _debouncedActive(false) {}

    Reading getReading() override {
        if (!_source) return Reading(0.0f, "", "ERR", 0, true);
        
        RawReading deviceRaw = _source->read();
        bool currentRawActive = _activeLow ? (deviceRaw.value == 0) : (deviceRaw.value != 0);
        
        unsigned long now = deviceRaw.timestamp;

        if (currentRawActive != _lastRawActive) {
            _lastDebounceTime = now;
        }

        if ((now - _lastDebounceTime) >= _debounceMs) {
            _debouncedActive = currentRawActive;
        }

        _lastRawActive = currentRawActive;
        float value = _debouncedActive ? 1.0f : 0.0f;
        
        return Reading(value, "", _debouncedActive ? "ON" : "OFF", 0, false);
    }

private:
    bool _activeLow;
    unsigned long _debounceMs;
    bool _lastRawActive;
    unsigned long _lastDebounceTime;
    bool _debouncedActive;
};

#endif
