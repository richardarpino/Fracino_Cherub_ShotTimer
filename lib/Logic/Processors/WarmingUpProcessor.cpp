#include "WarmingUpProcessor.h"
#include <Arduino.h>
#include <stdio.h>

WarmingUpProcessor::WarmingUpProcessor(ISensorRegistry* registry, unsigned long timeoutMs)
    : _registry(registry), _startTime(millis()), _timeoutMs(timeoutMs), _isFinished(false), _firstUpdate(true) {
    if (_registry) {
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
        Reading pressure = _registry->getLatest<BoilerPressureReading>();
        if (pressure.value > 0.1f) {
            _isFinished = true;
        }
    }

    // 1. Timeout Check
    if (!_isFinished && (millis() - _startTime >= _timeoutMs)) {
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
    
    unsigned long elapsed = millis() - _startTime;
    float timeProgress = (float)elapsed / (float)_timeoutMs * 100.0f;
    if (timeProgress > 100.0f) timeProgress = 100.0f;
    
    // Whichever is further along
    return (moveProgress > timeProgress) ? moveProgress : timeProgress;
}
