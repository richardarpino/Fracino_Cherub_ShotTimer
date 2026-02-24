#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "WarmingUpBlocker.h"
#include <SensorStub.h>
#include "StringUtils.h"

void test_initial_state() {
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&pressureSensor);
    
    TEST_ASSERT_FALSE(blocker.isActive());
    BlockerStatus status = blocker.getStatus();
    TEST_ASSERT_EQUAL_STRING("Warming Up...", status.title.c_str());
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.0bar", status.message.c_str());
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0f, status.progress);
}

void test_cycle_detection_with_noise() {
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&pressureSensor);
    
    // Initial noise at 0.1bar should NOT trigger a cycle
    pressureSensor.setReading(Reading(0.1, "BAR", "BOILER", 1, false));
    blocker.update();
    pressureSensor.setReading(Reading(0.05, "BAR", "BOILER", 1, false));
    blocker.update();
    TEST_ASSERT_EQUAL_INT(0, blocker.getStatus().progress); // No cycles counted

    // Cycle 1: Heat to 1.1, drop to 0.8
    pressureSensor.setReading(Reading(1.1, "BAR", "BOILER", 1, false));
    blocker.update();
    pressureSensor.setReading(Reading(0.8, "BAR", "BOILER", 1, false));
    blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 2, currently 0.8bar", blocker.getStatus().message.c_str());
    
    // Cycle 2: Heat to 1.1, drop to 0.8
    pressureSensor.setReading(Reading(1.1, "BAR", "BOILER", 1, false));
    blocker.update();
    pressureSensor.setReading(Reading(0.8, "BAR", "BOILER", 1, false));
    blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 3, currently 0.8bar", blocker.getStatus().message.c_str());

    // Cycle 3: Heat to 1.1, drop to 0.8 -> FINISHED
    pressureSensor.setReading(Reading(1.1, "BAR", "BOILER", 1, false));
    blocker.update();
    pressureSensor.setReading(Reading(0.8, "BAR", "BOILER", 1, false));
    blocker.update();
    
    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("WARM", blocker.getStatus().message.c_str());
}

void test_timeout() {
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&pressureSensor);
    
    // 0 minutes
    setMillis(0);
    blocker.update();
    TEST_ASSERT_FALSE(blocker.isActive());

    // 9 minutes (540,000 ms)
    setMillis(540000);
    blocker.update();
    TEST_ASSERT_FALSE(blocker.isActive());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 90.0f, blocker.getStatus().progress);

    // 10 minutes (600,000 ms)
    setMillis(600000);
    blocker.update();
    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 100.0f, blocker.getStatus().progress);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_cycle_detection_with_noise);
    RUN_TEST(test_timeout);
    return UNITY_END();
}
