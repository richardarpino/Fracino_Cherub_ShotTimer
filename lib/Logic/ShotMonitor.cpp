#include "ShotMonitor.h"

ShotMonitor::ShotMonitor(ManualPumpTimer* timer, ISensorRegistry* registry)
    : _pump(registry), _timer(timer), _registry(registry), _lastValidDuration(0.0f) {}

void ShotMonitor::update() {
    if (!_registry) return;

    _pump.update();

    if (_pump.justStarted()) {
        if (_timer) _timer->start();
    }

    if (_pump.justStopped()) {
        if (_timer) {
            float duration = _timer->getReading().value;
            _timer->stop();
            
            // Issue #10: Filter purges (< 10s)
            if (duration >= 10.0f) {
                _lastValidDuration = duration;
            }
        }
    }

    // Always publish derived results to the Registry
    if (_timer) {
        _registry->publish<ShotTimeTag>(_timer->getReading());
    }
    _registry->publish<LastValidShotTag>(Reading(_lastValidDuration, "SECS", "LAST SHOT", 1, false));
}
