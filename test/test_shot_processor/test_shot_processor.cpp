#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/Processors/ShotMonitorProcessor.h"
#include "../../lib/Logic/Processors/ShotMonitorProcessor.cpp"
#include "../_common/stubs/SensorStub.h"

void test_shot_timing() {
    SensorDispatcher registry;
    SensorStub pumpSensor;
    ShotMonitorProcessor processor(&registry, 0.1f); // 100ms compensation

    registry.provide<PumpReading>(&pumpSensor);
    registry.attachProcessor<ShotTimeReading>(&processor);

    // Initial state
    registry.publish<SystemUptimeReading>(1.0f);
    registry.publish<PumpReading>(0.0f);
    registry.update();

    // 1. Pump Start
    pumpSensor.setReading(1.0f); // ON
    registry.update();

    // 4 seconds later (1s -> 5s)
    registry.publish<SystemUptimeReading>(5.0f);
    registry.update(); 
    
    Reading shotTime = registry.getLatest<ShotTimeReading>();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 4.0f, shotTime.value); // Running time not compensated
    TEST_ASSERT_EQUAL_STRING("RUNNING", shotTime.label);

    // 2. Pump Stop
    registry.publish<SystemUptimeReading>(6.0f); // 5s elapsed
    pumpSensor.setReading(0.0f); // OFF
    registry.update();

    shotTime = registry.getLatest<ShotTimeReading>();
    TEST_ASSERT_EQUAL_FLOAT(4.9f, shotTime.value); // Final duration (5s - 0.1s)
    TEST_ASSERT_EQUAL_STRING("READY", shotTime.label);
}

void test_last_valid_shot_filtering() {
    SensorDispatcher registry;
    SensorStub pumpSensor;
    ShotMonitorProcessor processor(&registry, 0.1f); // 100ms compensation

    registry.provide<PumpReading>(&pumpSensor);
    registry.attachProcessor<ShotTimeReading>(&processor);

    // 1. Long Shot (Valid)
    registry.publish<SystemUptimeReading>(0.0f);
    pumpSensor.setReading(1.0f); registry.update();
    
    registry.publish<SystemUptimeReading>(15.0f); // 15s
    pumpSensor.setReading(0.0f); registry.update();

    Reading lastValid = registry.getLatest<LastValidShotReading>();
    TEST_ASSERT_EQUAL_FLOAT(14.9f, lastValid.value); // 15s - 0.1s

    // 2. Short Shot (Purge - should be ignored)
    registry.publish<SystemUptimeReading>(20.0f);
    pumpSensor.setReading(1.0f); registry.update();
    
    registry.publish<SystemUptimeReading>(25.0f); // 5s
    pumpSensor.setReading(0.0f); registry.update();

    lastValid = registry.getLatest<LastValidShotReading>();
    TEST_ASSERT_EQUAL_FLOAT(14.9f, lastValid.value); // Still 14.9s!
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_shot_timing);
    RUN_TEST(test_last_valid_shot_filtering);
    return UNITY_END();
}
