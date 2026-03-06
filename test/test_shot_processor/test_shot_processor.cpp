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
    ShotMonitorProcessor processor(&registry);

    registry.provide<PumpReading>(&pumpSensor);
    registry.attachProcessor<ShotTimeReading>(&processor);

    setMillis(1000);
    // 1. Pump Start
    pumpSensor.setReading(1.0f); // ON
    registry.update();

    setMillis(5000);
    registry.update(); // 4 seconds later
    
    Reading shotTime = registry.getLatest<ShotTimeReading>();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 4.0f, shotTime.value);
    TEST_ASSERT_EQUAL_STRING("RUNNING", shotTime.label);

    // 2. Pump Stop
    setMillis(6000);
    pumpSensor.setReading(0.0f); // OFF
    registry.update();

    shotTime = registry.getLatest<ShotTimeReading>();
    TEST_ASSERT_EQUAL_FLOAT(5.0f, shotTime.value); // Final duration
    TEST_ASSERT_EQUAL_STRING("READY", shotTime.label);
}

void test_last_valid_shot_filtering() {
    SensorDispatcher registry;
    SensorStub pumpSensor;
    ShotMonitorProcessor processor(&registry);

    registry.provide<PumpReading>(&pumpSensor);
    registry.attachProcessor<ShotTimeReading>(&processor);

    // 1. Long Shot (Valid)
    setMillis(0);
    pumpSensor.setReading(1.0f); registry.update();
    setMillis(15000); // 15s
    pumpSensor.setReading(0.0f); registry.update();

    Reading lastValid = registry.getLatest<LastValidShotReading>();
    TEST_ASSERT_EQUAL_FLOAT(15.0f, lastValid.value);

    // 2. Short Shot (Purge - should be ignored)
    setMillis(20000);
    pumpSensor.setReading(1.0f); registry.update();
    setMillis(25000); // 5s
    pumpSensor.setReading(0.0f); registry.update();

    lastValid = registry.getLatest<LastValidShotReading>();
    TEST_ASSERT_EQUAL_FLOAT(15.0f, lastValid.value); // Still 15s!
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_shot_timing);
    RUN_TEST(test_last_valid_shot_filtering);
    return UNITY_END();
}
