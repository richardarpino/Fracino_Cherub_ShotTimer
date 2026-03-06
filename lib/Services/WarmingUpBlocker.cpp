#include "WarmingUpBlocker.h"

WarmingUpBlocker::WarmingUpBlocker(ISensorRegistry* registry, unsigned long timeoutMs)
    : _registry(registry), _isFinished(false), _wasFinished(false) {
}

void WarmingUpBlocker::update() {
    _wasFinished = _isFinished;
    
    if (_registry) {
        StatusMessage status = _registry->getLatest<WarmingUpStatus>();
        // FAIL-SAFE: Unblock if ready (100%), or if failed, or if tag is missing (-1.0f)
        _isFinished = (status.progress >= 100.0f || status.isFailed || status.progress < 0.0f);
    }
}

StatusMessage WarmingUpBlocker::getStatus() const {
    if (_registry) {
        return _registry->getLatest<WarmingUpStatus>();
    }
    return StatusMessage("Error", "No Registry", 0.0f, true);
}

bool WarmingUpBlocker::isActive() const {
    return _isFinished;
}

bool WarmingUpBlocker::justStarted() const {
    return _isFinished && !_wasFinished;
}

bool WarmingUpBlocker::justStopped() const {
    return !_isFinished && _wasFinished;
}
