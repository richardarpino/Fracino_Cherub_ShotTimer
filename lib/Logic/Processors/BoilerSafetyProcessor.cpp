#include "BoilerSafetyProcessor.h"

BoilerSafetyProcessor::BoilerSafetyProcessor(ISensorRegistry* registry)
    : _registry(registry), _trigger(registry, 2.5f, 0.1f, ThresholdMode::ABOVE) {}

void BoilerSafetyProcessor::update() {
    if (!_registry) return;

    _trigger.update();
    
    if (_trigger.isActive()) {
        _registry->publish<BoilerSafetyStatus>(StatusMessage("Pressure", "DANGER: HIGH", 100.0f, false));
    } else {
        _registry->publish<BoilerSafetyStatus>(StatusMessage("Pressure", "OK", 0.0f, false));
    }
}
