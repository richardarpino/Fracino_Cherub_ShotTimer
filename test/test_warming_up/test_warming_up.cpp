#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Services/WarmingUpBlocker.h"
#include "../../lib/Services/WarmingUpBlocker.cpp"
#include <SensorStub.h>
#include "../../lib/Utils/StringUtils.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"

void test_initial_state() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    
    TEST_ASSERT_FALSE(blocker.isActive());
    StatusMessage status = blocker.getStatus();
    TEST_ASSERT_EQUAL_STRING("Warming Up...", status.title);
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.0bar", status.message);
    TEST_ASSERT_FLOAT_WITHIN(0.01, 0.0f, status.progress);
}

void test_zigzag_extrema_detection() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    
    // 1. Initial Ramp: 0.0 -> 1.1 (First Peak)
    for (int i = 0; i <= 11; i++) {
        float p = i / 10.0f;
        pressureSensor.setReading(Reading(p, "BAR", "BOILER", 1, false));
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 1.1bar", blocker.getStatus().message);

    // 2. First Drop: 1.1 -> 0.8 (First Valley)
    for (int i = 11; i >= 8; i--) {
        float p = i / 10.0f;
        pressureSensor.setReading(Reading(p, "BAR", "BOILER", 1, false));
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.8bar", blocker.getStatus().message);

    // 3. Second Heat: 0.8 -> 1.2 (Second Peak)
    for (int i = 8; i <= 12; i++) {
        float p = i / 10.0f;
        pressureSensor.setReading(Reading(p, "BAR", "BOILER", 1, false));
        blocker.update();
    }
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false));
    blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 2, currently 1.1bar", blocker.getStatus().message);

    // 4. Third Heat (Cycle 2 Completed)
    pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false)); blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 3, currently 1.1bar", blocker.getStatus().message);

    // 5. Fourth Heat (Cycle 3 Completed -> FINISHED)
    pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false)); blocker.update();

    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("WARM", blocker.getStatus().message);
    
    // Verify registry publishing (Using .progress)
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, registry.getLatest<WarmingUpTag>().progress);
}

void test_warm_startup() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false));
    
    WarmingUpBlocker blocker(&registry, &pressureSensor);
    blocker.update();
    
    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, registry.getLatest<WarmingUpTag>().progress);
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
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, registry.getLatest<WarmingUpTag>().progress);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_initial_state);
    RUN_TEST(test_zigzag_extrema_detection);
    RUN_TEST(test_warm_startup);
    RUN_TEST(test_timeout);
    return UNITY_END();
}
