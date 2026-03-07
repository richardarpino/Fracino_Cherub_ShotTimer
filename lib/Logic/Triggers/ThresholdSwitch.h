#ifndef THRESHOLD_SWITCH_H
#define THRESHOLD_SWITCH_H

#include "../../Interfaces/ISwitch.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"
#include <cstring>

/**
 * Reactive Trigger that turns continuous sensor data into a discrete binary state.
 * Supports hysteresis and physical unit validation.
 */
template<typename TTag>
class ThresholdSwitch : public ISwitch {
public:
    ThresholdSwitch(ISensorRegistry* registry, float threshold, float hysteresis = 0.0f, 
                    ThresholdMode mode = ThresholdMode::ABOVE, const char* expectedUnit = "")
        : _registry(registry), _threshold(threshold), _hysteresis(hysteresis), 
          _mode(mode), _expectedUnit(expectedUnit), 
          _isActive(false), _lastActive(false) {}

    void update() override {
        if (!_registry) return;

        _lastActive = _isActive;
        Reading reading = _registry->template getLatest<TTag>();

        // 1. Validation: Physics Mismatch
        if (reading.quantity != TTag::QUANTITY) {
            _isActive = false;
            return;
        }

        // 2. Validation: Unit Mismatch (If expectedUnit is set)
        if (_expectedUnit != nullptr && _expectedUnit[0] != '\0' && strcmp(reading.unit, _expectedUnit) != 0) {
            _isActive = false;
            return;
        }

        // 3. Logic: Hysteresis Comparison
        if (_mode == ThresholdMode::ABOVE) {
            if (!_isActive && reading.value >= _threshold) {
                _isActive = true;
            } else if (_isActive && reading.value < (_threshold - _hysteresis)) {
                _isActive = false;
            }
        } else { // BELOW
            if (!_isActive && reading.value <= _threshold) {
                _isActive = true;
            } else if (_isActive && reading.value > (_threshold + _hysteresis)) {
                _isActive = false;
            }
        }
    }

    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _isActive && !_lastActive; }
    bool justStopped() const override { return !_isActive && _lastActive; }

private:
    ISensorRegistry* _registry;
    float _threshold;
    float _hysteresis;
    ThresholdMode _mode;
    const char* _expectedUnit;
    bool _isActive;
    bool _lastActive;
};

#endif
