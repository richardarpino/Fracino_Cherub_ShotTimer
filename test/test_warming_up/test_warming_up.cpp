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

void test_initial_state() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    
    // Initial state check
    TEST_ASSERT_FALSE(blocker.isActive());
    StatusMessage status = blocker.getStatus();
    TEST_ASSERT_EQUAL_STRING("Warming Up...", status.title);
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.0bar", status.message);
}

void test_zigzag_reactive_flow() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    HeatingCycleProcessor processor(&registry);
    
    // Setup reactive chain
    registry.provide<BoilerPressureTag>(&pressureSensor);
    registry.attachProcessor<HeatingCycleTag>(&processor);
    
    // 1. Initial Ramp: 0.0 -> 1.1 (First Peak)
    for (int i = 0; i <= 11; i++) {
        pressureSensor.setReading(Reading(i / 10.0f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
        registry.update(); // Polling -> Resolve Boiler -> Resolve HeatingCycle
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 1.1bar", blocker.getStatus().message);

    // 2. First Drop: 1.1 -> 0.8 (First Valley)
    for (int i = 11; i >= 8; i--) {
        pressureSensor.setReading(Reading(i / 10.0f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
        registry.update();
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.8bar", blocker.getStatus().message);

    // 3. Second Heat: 0.8 -> 1.2 (Second Peak)
    for (int i = 8; i <= 12; i++) {
        pressureSensor.setReading(Reading(i / 10.0f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
        registry.update();
        blocker.update();
    }
    // Small dip to consolidate the cycle (The processor counts cycle on trend flip)
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
    registry.update();
    blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 2, currently 1.1bar", blocker.getStatus().message);

    // Simulation of more cycles to verify completion
    // Cycle 2 completion
    pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE)); registry.update(); blocker.update();
    pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE)); registry.update(); blocker.update();
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE)); registry.update(); blocker.update();
    
    // Cycle 3 completion (TARGET = 3)
    pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE)); registry.update(); blocker.update();
    pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE)); registry.update(); blocker.update();
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE)); registry.update(); blocker.update();

    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("WARM", blocker.getStatus().message);
}

void test_warm_startup_reactive() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    HeatingCycleProcessor processor(&registry);
    
    registry.provide<BoilerPressureTag>(&pressureSensor);
    registry.attachProcessor<HeatingCycleTag>(&processor);

    // Start already pressurized
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
    registry.update(); // Triggers Warm Start bypass in processor
    blocker.update();
    
    TEST_ASSERT_TRUE(blocker.isActive());
}

void test_timeout() {
    setMillis(0);
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    
    blocker.update();
    TEST_ASSERT_FALSE(blocker.isActive());

    setMillis(600000);
    blocker.update();
    TEST_ASSERT_TRUE(blocker.isActive());
}

void test_memory_limit_reactive() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    HeatingCycleProcessor processor(&registry);
    
    registry.provide<BoilerPressureTag>(&pressureSensor);
    registry.attachProcessor<HeatingCycleTag>(&processor);

    // 20 heating cycles
    for (int cycle = 1; cycle <= 20; cycle++) {
        pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
        registry.update();
        pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE));
        registry.update();
    }

    // Check that it didn't grow unbounded
    TEST_ASSERT_TRUE(processor._moves.size() <= 11); // 1 initial + 5 cycles (10 moves) max
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
