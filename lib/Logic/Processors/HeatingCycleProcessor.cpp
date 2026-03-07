#include "HeatingCycleProcessor.h"
#include <Arduino.h>

HeatingCycleProcessor::HeatingCycleProcessor(ISensorRegistry* registry)
    : _registry(registry), _lastRoundedReading(-1.0f), _totalCompletedCycles(0) {}

void HeatingCycleProcessor::update() {
    if (!_registry) return;
    
    Reading pressure = _registry->getLatest<BoilerPressureReading>();
    processHistory(pressure.value);
    
    // Publish derived state
    _registry->publish<HeatingCycleReading>((float)_totalCompletedCycles);
}

void HeatingCycleProcessor::processHistory(float pressure) {
    // Round to 1 decimal place (e.g. 1.1)
    float rounded = (int)(pressure * 10 + 0.5f) / 10.0f;
    
    // Only process if it's a distinct 0.1 bar step
    if (rounded == _lastRoundedReading) return;

    if (_moves.empty()) {
        _moves.push_back({});
    }

    // Handle Warm Start:
    // If first move, empty, and already pressurized
    if (_moves.size() == 1 && _moves.back().empty() && rounded > 0.1f) {
        // We no longer jump to 3 cycles. 
        // We fall through to ensure history is populated.
    }

    std::vector<float>& currentMove = _moves.back();
    
    if (currentMove.empty()) {
        currentMove.push_back(rounded);
        _lastRoundedReading = rounded;
        return;
    }

    bool roundingUp = (rounded > currentMove.back());
    bool wasRising = true;

    if (currentMove.size() >= 2) {
        wasRising = (currentMove.back() > currentMove[currentMove.size()-2]);
    } else if (_moves.size() >= 2) {
        std::vector<float>& prevMove = _moves[_moves.size()-2];
        wasRising = (currentMove.back() > prevMove.back());
    }

    if (roundingUp != wasRising && currentMove.size() >= 1) {
        _moves.push_back({rounded});
        
        if (_moves.size() % 2 != 0 && _moves.size() > 1) {
            _totalCompletedCycles++;
        }

        if (_moves.size() > (size_t)MAX_HISTORY_MOVES) {
            _moves.erase(_moves.begin() + 1, _moves.begin() + 3);
        }
    } else {
        currentMove.push_back(rounded);
    }

    _lastRoundedReading = rounded;
}
