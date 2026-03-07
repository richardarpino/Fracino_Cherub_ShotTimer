#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Services/WarmingUpBlocker.h"
#include "../../lib/Services/WarmingUpBlocker.cpp"
#include <SensorStub.h>
#include "../../lib/Utils/StringUtils.h"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/Processors/HeatingCycleProcessor.h"
#include "../../lib/Logic/Processors/HeatingCycleProcessor.cpp"
#include "../../lib/Logic/Processors/WarmingUpProcessor.h"
#include "../../lib/Logic/Processors/WarmingUpProcessor.cpp"

void test_initial_state() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry);
    WarmingUpProcessor processor(&registry);
    registry.attachProcessor<WarmingUpStatus>(&processor);
    
    // Initial state check
    TEST_ASSERT_FALSE(blocker.isActive());
    StatusMessage status = blocker.getStatus();
    TEST_ASSERT_EQUAL_STRING("Warming Up...", status.title);
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.0bar", status.message);
}

void test_zigzag_reactive_flow() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry);
    HeatingCycleProcessor cycleProc(&registry);
    WarmingUpProcessor warmProc(&registry);
    
    // Setup reactive chain
    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.attachProcessor<HeatingCycleReading>(&cycleProc);
    registry.attachProcessor<WarmingUpStatus>(&warmProc);
    
    // 1. Initial Ramp: 0.0 -> 1.1 (First Peak)
    for (int i = 0; i <= 11; i++) {
        pressureSensor.setReading(i / 10.0f);
        registry.update(); // Polling -> Resolve Boiler -> Resolve HeatingCycle
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 1.1bar", blocker.getStatus().message);

    // 2. First Drop: 1.1 -> 0.8 (First Valley)
    for (int i = 11; i >= 8; i--) {
        pressureSensor.setReading(i / 10.0f);
        registry.update();
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.8bar", blocker.getStatus().message);

    // 3. Second Heat: 0.8 -> 1.2 (Second Peak)
    for (int i = 8; i <= 12; i++) {
        pressureSensor.setReading(i / 10.0f);
        registry.update();
        blocker.update();
    }
    // Simulation of more cycles to verify completion
    pressureSensor.setReading(0.8f); registry.update(); blocker.update();
    pressureSensor.setReading(1.2f); registry.update(); blocker.update();
    pressureSensor.setReading(1.1f); registry.update(); blocker.update();
    
    pressureSensor.setReading(0.8f); registry.update(); blocker.update();
    pressureSensor.setReading(1.2f); registry.update(); blocker.update();
    pressureSensor.setReading(1.1f); registry.update(); blocker.update();

    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("WARM", blocker.getStatus().message);
}

void test_warm_startup_reactive() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry);
    HeatingCycleProcessor cycleProc(&registry);
    WarmingUpProcessor warmProc(&registry);
    
    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.attachProcessor<HeatingCycleReading>(&cycleProc);
    registry.attachProcessor<WarmingUpStatus>(&warmProc);

    // Start already pressurized
    pressureSensor.setReading(1.1f);
    registry.update(); // Triggers Warm Start bypass in processor
    blocker.update();
    
    TEST_ASSERT_TRUE(blocker.isActive());
}

void test_timeout() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry);
    HeatingCycleProcessor cycleProc(&registry);
    WarmingUpProcessor processor(&registry);
    
    registry.attachProcessor<HeatingCycleReading>(&cycleProc);
    registry.attachProcessor<WarmingUpStatus>(&processor);
    
    // Initial uptime and pressure (cold)
    registry.publish<SystemUptimeReading>(0.0f);
    registry.publish<BoilerPressureReading>(0.0f);

    registry.update(); // Clear firstUpdate flag if it exists (processor might check it)
    blocker.update();
    TEST_ASSERT_FALSE(blocker.isActive());

    // Travel to 10 minutes (600 seconds)
    registry.publish<SystemUptimeReading>(600.0f);

    // Trigger update (without pressure skip)
    registry.publish<BoilerPressureReading>(0.05f); // Still below 0.1f skip threshold
    blocker.update();
    
    // This should FAIL (RED) until WarmingUpProcessor is refactored to use SystemUptimeReading
    TEST_ASSERT_TRUE(blocker.isActive());
}

void test_memory_limit_reactive() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry);
    HeatingCycleProcessor cycleProc(&registry);
    
    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.attachProcessor<HeatingCycleReading>(&cycleProc);

    // 20 heating cycles
    for (int cycle = 1; cycle <= 20; cycle++) {
        pressureSensor.setReading(0.8f);
        registry.update();
        pressureSensor.setReading(1.2f);
        registry.update();
    }

    // Check that it didn't grow unbounded
    TEST_ASSERT_TRUE(cycleProc._moves.size() <= 11); // 1 initial + 5 cycles (10 moves) max
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_zigzag_reactive_flow);
    RUN_TEST(test_warm_startup_reactive);
    RUN_TEST(test_timeout);
    RUN_TEST(test_memory_limit_reactive);
    return UNITY_END();
}
