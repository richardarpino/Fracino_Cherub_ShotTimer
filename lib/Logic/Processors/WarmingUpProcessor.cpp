#include "WarmingUpProcessor.h"
#include "../../Utils/StringUtils.h"
#include <Arduino.h>
#include <stdio.h>

WarmingUpProcessor::WarmingUpProcessor(ISensorRegistry* registry, unsigned long timeoutMs)
    : _registry(registry), _startTime(millis()), _timeoutMs(timeoutMs), _isFinished(false) {
    if (_registry) {
        _registry->publish<WarmingUpStatus>(StatusMessage("Warming Up...", "Heating Cycle 1, currently 0.0bar", 0.0f, false));
    }
}

void WarmingUpProcessor::update() {
    if (!_registry) return;

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
    float pressure = _registry->getLatest<BoilerPressureReading>().value;
    
    if (_isFinished) {
        _registry->publish<WarmingUpStatus>(StatusMessage("Ready", "WARM", 100.0f, false));
        return;
    }

    int currentCycles = (int)cycles.value;
    int displayCycle = (currentCycles < TARGET_CYCLES) ? (currentCycles + 1) : TARGET_CYCLES;
    
    char valBuf[16];
    StringUtils::formatFloat1(valBuf, sizeof(valBuf), pressure);
    
    snprintf(_statusBuffer, sizeof(_statusBuffer), "Heating Cycle %d, currently %sbar", displayCycle, valBuf);
    
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
