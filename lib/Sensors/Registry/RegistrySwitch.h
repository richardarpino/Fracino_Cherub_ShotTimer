#ifndef REGISTRY_SWITCH_H
#define REGISTRY_SWITCH_H

#include "../../Interfaces/ISwitch.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTypes.h"

/**
 * A Virtual Switch that pulls its state from the Sensor Registry.
 * Allows multiple orchestrators to share the same hardware state
 * while maintaining their own independent edge detection (justStarted/justStopped).
 */
template<typename TTag>
class RegistrySwitch : public ISwitch {
public:
    RegistrySwitch(ISensorRegistry* registry) 
        : _registry(registry), _isActive(false), _lastIsActive(false) {}

    void update() override {
        if (!_registry) return;
        _lastIsActive = _isActive;
        updateInternal(typename TTag::DataType{});
    }

private:
    void updateInternal(Reading) {
        Reading data = _registry->template getLatest<TTag>();
        _isActive = (data.value > 0.5f) && !data.isError;
    }

    void updateInternal(StatusMessage) {
        StatusMessage data = _registry->template getLatest<TTag>();
        _isActive = (data.progress >= 100.0f) && !data.isFailed;
    }

public:
    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _isActive && !_lastIsActive; }
    bool justStopped() const override { return !_isActive && _lastIsActive; }

private:
    ISensorRegistry* _registry;
    bool _isActive;
    bool _lastIsActive;
};

#endif
