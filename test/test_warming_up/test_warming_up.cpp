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

void test_zigzag_extrema_detection() {
    SensorStub pressureSensor;
    WarmingUpBlocker blocker(&pressureSensor);
    
    // 1. Initial Ramp: 0.0 -> 1.1 (First Peak)
    // Should be registered but ignored for cycle count
    for (int i = 0; i <= 11; i++) {
        float p = i / 10.0f;
        pressureSensor.setReading(Reading(p, "BAR", "BOILER", 1, false));
        blocker.update();
    }
    // We confirm we see "Cycle 1" (meaning we haven't completed a cycle yet)
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 1.1bar", blocker.getStatus().message.c_str());

    // 2. First Drop: 1.1 -> 0.8 (First Valley)
    for (int i = 11; i >= 8; i--) {
        float p = i / 10.0f;
        pressureSensor.setReading(Reading(p, "BAR", "BOILER", 1, false));
        blocker.update();
    }
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 1, currently 0.8bar", blocker.getStatus().message.c_str());

    // 3. Second Heat: 0.8 -> 1.2 (Second Peak - CONFIRMED)
    // Note: Peak height varies (1.2 instead of 1.1)
    for (int i = 8; i <= 12; i++) {
        float p = i / 10.0f;
        pressureSensor.setReading(Reading(p, "BAR", "BOILER", 1, false));
        blocker.update();
    }
    // Important: The peak is confirmed once we see a drop OR once we hit the peak?
    // User synopsis says: "encounter the highest number... revealed by the next number".
    // So hitting 1.2 is the peak, but we might only count it when we see 1.1 next.
    // Let's assume we count it immediately for progress, but let's see.
    // Based on "encounter the highest... which is when we pass it", let's drop.
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false));
    blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 2, currently 1.1bar", blocker.getStatus().message.c_str());

    // 4. Third Heat (Cycle 2 Completed)
    pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false)); blocker.update();
    TEST_ASSERT_EQUAL_STRING("Heating Cycle 3, currently 1.1bar", blocker.getStatus().message.c_str());
    printf("DEBUG: Progress at Cycle 2: %.1f%%\n", blocker.getStatus().progress);

    // 5. Fourth Heat (Cycle 3 Completed -> FINISHED)
    pressureSensor.setReading(Reading(0.8f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.2f, "BAR", "BOILER", 1, false)); blocker.update();
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false)); blocker.update();

    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("WARM", blocker.getStatus().message.c_str());
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 100.0f, blocker.getStatus().progress);
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

void test_warm_startup() {
    SensorStub pressureSensor;
    // Initial reading is already 1.1 bar
    pressureSensor.setReading(Reading(1.1f, "BAR", "BOILER", 1, false));
    
    WarmingUpBlocker blocker(&pressureSensor);
    blocker.update();
    
    // Should immediately be active (bypass warmup)
    TEST_ASSERT_TRUE(blocker.isActive());
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
    RUN_TEST(test_zigzag_extrema_detection);
    RUN_TEST(test_warm_startup);
    RUN_TEST(test_timeout);
    return UNITY_END();
}
