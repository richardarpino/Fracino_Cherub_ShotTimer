#include "OTABlocker.h"

OTABlocker::OTABlocker(ISensorRegistry* registry) 
    : _registry(registry), _isActive(false), _wasActive(false) {
}

void OTABlocker::update() {
    _wasActive = _isActive;
    if (_registry) {
        StatusMessage status = _registry->getLatest<OTAStatus>();
        // isActive means "Gate is OPEN" (not blocking).
        // It is OPEN if progress is 100% or if it has failed (fail-safe).
        _isActive = (status.progress >= 100.0f || status.isFailed);
    }
}

StatusMessage OTABlocker::getStatus() const {
    if (_registry) {
        return _registry->getLatest<OTAStatus>();
    }
    return StatusMessage("OTA", "NO REGISTRY", 0, true);
}

bool OTABlocker::isActive() const {
    return _isActive;
}

bool OTABlocker::justStarted() const {
    return _isActive && !_wasActive;
}

bool OTABlocker::justStopped() const {
    return !_isActive && _wasActive;
}
