#include "ShotMonitorProcessor.h"
#include <Arduino.h>

ShotMonitorProcessor::ShotMonitorProcessor(ISensorRegistry* registry)
    : _registry(registry), _pump(registry), 
      _startTimeSecs(0.0f), _isRunning(false), 
      _lastDuration(0.0f), _lastValidDuration(0.0f) {}

void ShotMonitorProcessor::update() {
    if (!_registry) return;

    _pump.update();

    if (_pump.justStarted()) {
        startTimer();
    }

    if (_pump.justStopped()) {
        float duration = getElapsedSeconds();
        stopTimer();
        
        // Filter purges (< 10s)
        if (duration >= 10.0f) {
            _lastValidDuration = duration;
        }
    }

    // Always publish derived results to the Registry
    if (_isRunning || _lastDuration == 0.0f) {
        // Simple float publication (Registry handles 'RUNNING'/'READY' based on value)
        _registry->publish<ShotTimeReading>(getElapsedSeconds());
    } else {
        // Explicit publication for the "Final Duration" state to force 'READY' label
        SensorMetadata meta = _registry->getMetadata<ShotTimeReading>();
        Reading r = meta.init;
        r.value = _lastDuration;
        _registry->publish<ShotTimeReading>(r);
    }
    _registry->publish<LastValidShotReading>(_lastValidDuration);
}

void ShotMonitorProcessor::startTimer() {
    _isRunning = true;
    _startTimeSecs = _registry->getLatest<SystemUptimeReading>().value;
    _lastDuration = 0.0f;
}

void ShotMonitorProcessor::stopTimer() {
    if (!_isRunning) return;
    _lastDuration = getElapsedSeconds();
    _isRunning = false;
}

float ShotMonitorProcessor::getElapsedSeconds() const {
    if (!_isRunning) return _lastDuration;
    float now = _registry->getLatest<SystemUptimeReading>().value;
    return now - _startTimeSecs;
}
