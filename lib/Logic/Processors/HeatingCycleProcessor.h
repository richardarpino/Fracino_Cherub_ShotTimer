#ifndef HEATING_CYCLE_PROCESSOR_H
#define HEATING_CYCLE_PROCESSOR_H

#include "../../Interfaces/ITagProcessor.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"
#include <vector>

/**
 * Calculates heating cycles from BoilerPressureTag and publishes to HeatingCycleTag.
 * Encapsulates the zigzag pressure logic to satisfy SRP.
 */
class HeatingCycleProcessor : public ITagProcessor {
    friend void test_memory_limit_reactive(); // Allow test to inspect internal state
public:
    HeatingCycleProcessor(ISensorRegistry* registry);
    
    void update() override;

private:
    void processHistory(float pressure);

    ISensorRegistry* _registry;
    
    // Dimensional History
    std::vector<std::vector<float>> _moves;
    float _lastRoundedReading;
    int _totalCompletedCycles;
    
    const int MAX_HISTORY_MOVES = 11; // 1 initial + 5 cycles (up/down)
};

#endif
