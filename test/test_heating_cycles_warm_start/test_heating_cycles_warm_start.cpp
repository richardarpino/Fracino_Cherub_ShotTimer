#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/Processors/HeatingCycleProcessor.h"
#include "../../lib/Logic/Processors/HeatingCycleProcessor.cpp"
#include <SensorStub.h>

void test_warm_start_and_continue_counting() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    HeatingCycleProcessor cycleProc(&registry);
    
    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.attachProcessor<HeatingCycleReading>(&cycleProc);

    // 1. Start Warm (Pressure > 0.1)
    pressureSensor.setReading(1.1f);
    registry.update();
    
    // Now expecting 0 cycles initially (semantically correct)
    Reading cycles = registry.getLatest<HeatingCycleReading>();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, cycles.value);

    // 2. Perform a cooling phase (1.1 -> 0.8)
    for (float p = 1.1f; p >= 0.8f; p -= 0.1f) {
        pressureSensor.setReading(p);
        registry.update();
    }
    
    // Cycles should still be 0
    cycles = registry.getLatest<HeatingCycleReading>();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, cycles.value);

    // 3. Perform a heating phase (0.8 -> 1.2)
    for (float p = 0.8f; p <= 1.2f; p += 0.1f) {
        pressureSensor.setReading(p);
        registry.update();
    }

    // Now a full cycle (down then up) has completed.
    // The algo should have incremented to 1.
    cycles = registry.getLatest<HeatingCycleReading>();
    TEST_ASSERT_EQUAL_FLOAT(1.0f, cycles.value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_warm_start_and_continue_counting);
    return UNITY_END();
}
