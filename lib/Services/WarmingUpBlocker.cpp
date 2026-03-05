#include "WarmingUpBlocker.h"
#include "../Utils/StringUtils.h"

WarmingUpBlocker::WarmingUpBlocker(ISensorRegistry* registry, HardwareSensor* pressureSensor, unsigned long timeoutMs)
    : _registry(registry), _pressureSensor(pressureSensor), _lastPressure(0.0f), 
      _startTime(millis()), _timeoutMs(timeoutMs),       _isFinished(false), _wasFinished(false),
       _totalCompletedCycles(0),
      _lastRoundedReading(-1.0f) {
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
    
    // if (_isFinished) return;

    // Timeout check
    if (!_isFinished && millis() - _startTime >= _timeoutMs) {
        _isFinished = true;
    }

    if (!_pressureSensor) return;
    
    Reading r = _pressureSensor->getReading();
    float currentPressure = r.value;
    
    processHistory(currentPressure);

    _lastPressure = currentPressure;

    if (_registry) {
        _registry->publish<WarmingUpTag>(getStatus());
        _registry->publish<HeatingCycleTag>(Reading((float)_totalCompletedCycles, "", "CYCLES", 0, false, PhysicalQuantity::COUNTER));
    }

    if (_cycleTrigger) {
        _cycleTrigger->update();
        if (_cycleTrigger->isActive()) {
            _isFinished = true;
        }
    }
}

void WarmingUpBlocker::processHistory(float pressure) {
    // Round to 1 decimal place (e.g. 1.1)
    float rounded = (int)(pressure * 10 + 0.5f) / 10.0f;
    
    // Only process if it's a distinct 0.1 bar step
    if (rounded == _lastRoundedReading) return;

    if (_moves.empty()) {
        _moves.push_back({});
    }

    // Handle Warm Start Bypass:
    // If first move, empty, and already pressurized
    if (_moves.size() == 1 && _moves.back().empty() && rounded > 0.3f) {
        _isFinished = true;
        _lastRoundedReading = rounded;
        return;
    }

    std::vector<float>& currentMove = _moves.back();
    
    // If current move is empty, add first point
    if (currentMove.empty()) {
        currentMove.push_back(rounded);
        _lastRoundedReading = rounded;
        return;
    }

    // Determine current direction from the last move's trend
    // or from the first two points of the current move.
    bool roundingUp = (rounded > currentMove.back());
    bool wasRising = true; // Default for first move or if move has 1 point

    if (currentMove.size() >= 2) {
        wasRising = (currentMove.back() > currentMove[currentMove.size()-2]);
    } else if (_moves.size() >= 2) {
        // Look at previous move's last two points to determine direction flip
        // (Move N ends where Move N+1 starts)
        std::vector<float>& prevMove = _moves[_moves.size()-2];
        wasRising = (currentMove.back() > prevMove.back());
    }

    // Direction flip?
    if (roundingUp != wasRising && currentMove.size() >= 1) {
        // Start a new dimension
        _moves.push_back({rounded});
        
        // If we just flipped to UP (move index is even: 2, 4, 6...)
        // Cycle is completed when an UP trend begins
        if (_moves.size() % 2 != 0 && _moves.size() > 1) {
            _totalCompletedCycles++;
        }

        // Trimming: Keep Move 0 (initial) and the last N cycles
        if (_moves.size() > (size_t)MAX_HISTORY_MOVES) {
            // Remove the oldest Down/Up pair (moves 1 and 2)
            _moves.erase(_moves.begin() + 1, _moves.begin() + 3);
        }
    } else {
        currentMove.push_back(rounded);
    }

    _lastRoundedReading = rounded;
}

StatusMessage WarmingUpBlocker::getStatus() const {
    if (isActive()) {
        return StatusMessage("Ready", "WARM", 100.0f, false);
    }

    int displayCycle = (_totalCompletedCycles < TARGET_CYCLES) ? (_totalCompletedCycles + 1) : TARGET_CYCLES;
    
    char valBuf[16];
    StringUtils::formatFloat1(valBuf, sizeof(valBuf), _lastPressure);
    
    snprintf(_statusBuffer, sizeof(_statusBuffer), "Heating Cycle %d, currently %sbar", displayCycle, valBuf);
    
    return StatusMessage("Warming Up...", _statusBuffer, getProgress(), false);
}

float WarmingUpBlocker::getProgress() const {
    if (_isFinished) return 100.0f;

    // Progress is based on moves (80%) and time (20% fallback)
    // Total expected moves = 1 (Initial Ramp) + 2 * TARGET_CYCLES (Down/Up cycles)
    // Total expected moves = 1 (Initial Ramp) + 2 * TARGET_CYCLES (Down/Up cycles)
    const int totalExpectedMoves = 1 + (2 * TARGET_CYCLES); 
    
    if (_moves.empty()) return 0.0f;

    // We use the total perceived moves (initial + 2*cycles + 1 if currently falling)
    // Actually, simpler: progress based on _totalCompletedCycles
    float moveProgress = ((float)_totalCompletedCycles / (float)TARGET_CYCLES) * 100.0f;
    
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
