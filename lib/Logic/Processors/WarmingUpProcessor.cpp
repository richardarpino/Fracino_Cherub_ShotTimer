#include "WarmingUpProcessor.h"
#include <Arduino.h>
#include <stdio.h>

WarmingUpProcessor::WarmingUpProcessor(ISensorRegistry* registry, unsigned long timeoutMs)
    : _registry(registry), _startTimeSecs(0.0f), _timeoutSecs(timeoutMs / 1000.0f), _isFinished(false), _firstUpdate(true) {
    if (_registry) {
        _startTimeSecs = _registry->getLatest<SystemUptimeReading>().value;
        char valBuf[16];
        _registry->getLatest<BoilerPressureReading>().format(valBuf, sizeof(valBuf));
        snprintf(_statusBuffer, sizeof(_statusBuffer), "Heating Cycle 1, currently %s", valBuf);
        _registry->publish<WarmingUpStatus>(StatusMessage("Warming Up...", _statusBuffer, 0.0f, false));
    }
}

void WarmingUpProcessor::update() {
    if (!_registry) return;

    if (_firstUpdate) {
        _firstUpdate = false;
        _startTimeSecs = _registry->getLatest<SystemUptimeReading>().value;
        Reading pressure = _registry->getLatest<BoilerPressureReading>();
        if (pressure.value > 0.1f) {
            _isFinished = true;
        }
    }

    // 1. Timeout Check
    float now = _registry->getLatest<SystemUptimeReading>().value;
    if (!_isFinished && (now - _startTimeSecs >= _timeoutSecs)) {
        _isFinished = true;
    }

    // 2. Cycle Check
    Reading cycles = _registry->getLatest<HeatingCycleReading>();
    if (cycles.value >= TARGET_CYCLES) {
        _isFinished = true;
    }

    // 3. Status Construction
    Reading pressureReading = _registry->getLatest<BoilerPressureReading>();
    
    if (_isFinished) {
        _registry->publish<WarmingUpStatus>(StatusMessage("Ready", "WARM", 100.0f, false));
        return;
    }

    int currentCycles = (int)cycles.value;
    int displayCycle = (currentCycles < TARGET_CYCLES) ? (currentCycles + 1) : TARGET_CYCLES;
    
    char valBuf[16];
    pressureReading.format(valBuf, sizeof(valBuf));
    
    snprintf(_statusBuffer, sizeof(_statusBuffer), "Heating Cycle %d, currently %s", displayCycle, valBuf);
    
    _registry->publish<WarmingUpStatus>(StatusMessage("Warming Up...", _statusBuffer, getProgress(), false));
}

float WarmingUpProcessor::getProgress() const {
    if (_isFinished) return 100.0f;

    Reading reading = _registry->getLatest<HeatingCycleReading>();
    int currentCycles = (int)reading.value;
    float moveProgress = ((float)currentCycles / (float)TARGET_CYCLES) * 100.0f;
    
    float now = _registry->getLatest<SystemUptimeReading>().value;
    float elapsed = now - _startTimeSecs;
    float timeProgress = elapsed / _timeoutSecs * 100.0f;
    if (timeProgress > 100.0f) timeProgress = 100.0f;
    
    // Whichever is further along
    return (moveProgress > timeProgress) ? moveProgress : timeProgress;
}
