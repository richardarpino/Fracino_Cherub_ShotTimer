#include "WarmingUpBlocker.h"
#include "../Utils/StringUtils.h"

WarmingUpBlocker::WarmingUpBlocker(ISensor* pressureSensor, unsigned long timeoutMs)
    : _pressureSensor(pressureSensor), _cycleCount(0), _lastPressure(0.0f), 
      _currentPeak(0.0f), _startTime(millis()), _timeoutMs(timeoutMs), 
      _isFinished(false), _wasFinished(false) {}

void WarmingUpBlocker::update() {
    _wasFinished = _isFinished;
    
    if (_isFinished) return;

    // Timeout check
    if (millis() - _startTime >= _timeoutMs) {
        _isFinished = true;
        return;
    }

    if (!_pressureSensor) return;
    
    Reading r = _pressureSensor->getReading();
    float currentPressure = r.value;

    // Track peak within current cycle
    if (currentPressure > _currentPeak) {
        _currentPeak = currentPressure;
    }

    // Peak detection: look for a meaningful drop (e.g. 0.15bar)
    // The user sees 1.1 -> 0.8 (0.3 bar drop)
    const float DROP_THRESHOLD = 0.15f;
    if (_currentPeak > 0.1f && (currentPressure <= (_currentPeak - DROP_THRESHOLD))) {
        _cycleCount++;
        _currentPeak = currentPressure; // Reset peak for next cycle
        
        if (_cycleCount >= TARGET_CYCLES) {
            _isFinished = true;
        }
    }

    _lastPressure = currentPressure;
}

BlockerStatus WarmingUpBlocker::getStatus() const {
    if (isActive()) {
        return BlockerStatus("Warming Up...", "WARM", 100.0f, false);
    }

    char buf[64];
    int displayCycle = (_cycleCount < TARGET_CYCLES) ? (_cycleCount + 1) : TARGET_CYCLES;
    
    char valBuf[16];
    StringUtils::formatFloat1(valBuf, sizeof(valBuf), _lastPressure);
    
    snprintf(buf, sizeof(buf), "Heating Cycle %d, currently %sbar", displayCycle, valBuf);
    
    return BlockerStatus("Warming Up...", String(buf), getProgress(), false);
}

float WarmingUpBlocker::getProgress() const {
    // Progress is based on cycles (80%) and time (20% fallback)
    float cycleProgress = (float)_cycleCount / (float)TARGET_CYCLES * 100.0f;
    
    unsigned long elapsed = millis() - _startTime;
    float timeProgress = (float)elapsed / (float)_timeoutMs * 100.0f;
    if (timeProgress > 100.0f) timeProgress = 100.0f;
    
    // Whichever is further along, but mostly driven by cycles
    return (cycleProgress > timeProgress) ? cycleProgress : timeProgress;
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
