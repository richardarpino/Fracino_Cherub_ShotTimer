#include "WarmingUpBlocker.h"
#include "../Utils/StringUtils.h"

WarmingUpBlocker::WarmingUpBlocker(ISensorRegistry* registry, HardwareSensor* pressureSensor, unsigned long timeoutMs)
    : _registry(registry), _pressureSensor(pressureSensor), _lastPressure(0.0f), 
      _startTime(millis()), _timeoutMs(timeoutMs), _isFinished(false), _wasFinished(false) {
    _cycleTrigger = new ThresholdSwitch<HeatingCycleTag>(_registry, (float)TARGET_CYCLES, 0.0f, ThresholdMode::ABOVE);
    if (_registry) {
        _registry->publish<WarmingUpTag>(StatusMessage("Heating", "WARMING UP", 0.0f, false));
    }
}

WarmingUpBlocker::~WarmingUpBlocker() {
    if (_cycleTrigger) delete _cycleTrigger;
}

void WarmingUpBlocker::update() {
    _wasFinished = _isFinished;
    
    // Timeout check
    if (!_isFinished && millis() - _startTime >= _timeoutMs) {
        _isFinished = true;
    }

    if (_pressureSensor) {
        _lastPressure = _pressureSensor->getReading().value;
    }

    if (_registry) {
        _registry->publish<WarmingUpTag>(getStatus());
    }

    if (_cycleTrigger) {
        _cycleTrigger->update();
        if (_cycleTrigger->isActive()) {
            _isFinished = true;
        }
    }
}


StatusMessage WarmingUpBlocker::getStatus() const {
    if (isActive()) {
        return StatusMessage("Ready", "WARM", 100.0f, false);
    }

    Reading cycles = _registry->getLatest<HeatingCycleTag>();
    int currentCycles = (int)cycles.value;
    int displayCycle = (currentCycles < TARGET_CYCLES) ? (currentCycles + 1) : TARGET_CYCLES;
    
    char valBuf[16];
    StringUtils::formatFloat1(valBuf, sizeof(valBuf), _lastPressure);
    
    snprintf(_statusBuffer, sizeof(_statusBuffer), "Heating Cycle %d, currently %sbar", displayCycle, valBuf);
    
    return StatusMessage("Warming Up...", _statusBuffer, getProgress(), false);
}

float WarmingUpBlocker::getProgress() const {
    if (_isFinished) return 100.0f;

    Reading reading = _registry->getLatest<HeatingCycleTag>();
    int currentCycles = (int)reading.value;
    float moveProgress = ((float)currentCycles / (float)TARGET_CYCLES) * 100.0f;
    
    unsigned long elapsed = millis() - _startTime;
    float timeProgress = (float)elapsed / (float)_timeoutMs * 100.0f;
    if (timeProgress > 100.0f) timeProgress = 100.0f;
    
    // Whichever is further along, but mostly driven by moves
    return (moveProgress > timeProgress) ? moveProgress : timeProgress;
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
