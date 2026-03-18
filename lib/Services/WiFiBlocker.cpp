#include "WiFiBlocker.h"

WiFiBlocker::WiFiBlocker(ISensorRegistry* registry)
    : _registry(registry), _isActive(false), _wasActive(false) {}

void WiFiBlocker::update() {
    _wasActive = _isActive;
    if (_registry) {
        StatusMessage status = _registry->getLatest<WiFiStatus>();
        // Unblock if ready (100%) or if failed (fail-safe)
        // Note: we do NOT unblock on -1.0f (connecting)
        _isActive = (status.progress >= 100.0f || status.isFailed);
    }
}

StatusMessage WiFiBlocker::getStatus() const {
    if (_registry) return _registry->getLatest<WiFiStatus>();
    return StatusMessage();
}

bool WiFiBlocker::isActive() const { return _isActive; }
bool WiFiBlocker::justStarted() const { return _isActive && !_wasActive; }
bool WiFiBlocker::justStopped() const { return !_isActive && _wasActive; }
