#include "ShotMonitor.h"

ShotMonitor::ShotMonitor(ISwitch* pump, ManualPumpTimer* timer, ISensorRegistry* registry)
    : _pump(pump), _timer(timer), _registry(registry), _lastValidDuration(0.0f) {}

void ShotMonitor::update() {
    if (!_pump || !_registry) return;

    _pump->update();

    if (_pump->justStarted()) {
        if (_timer) _timer->start();
    }

    if (_pump->justStopped()) {
        if (_timer) {
            _timer->stop();
            // Business Rule: Shots < 10s are purges, discard them for "Last Good Shot"
            float duration = _timer->getReading().value;
            if (duration >= 10.0f) {
                _lastValidDuration = duration;
            }
        }
    }

    // Publish current duration
    if (_timer) {
        _registry->publish<ShotTimeTag>(_timer->getReading());
    }

    // Publish last valid duration (No filtering logic yet - this will fail the tests)
    _registry->publish<LastValidShotTag>(Reading(_lastValidDuration, "SECS", "LAST SHOT", 1, false));
}
