#include <unity.h>
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "Logic/Triggers/ThresholdSwitch.h"
#include "Logic/Triggers/LogicalTriggers.h"

void test_reading_has_quantity() {
    Reading r(1.2f, "BAR", "TEST", 1, false, PhysicalQuantity::PRESSURE);
    TEST_ASSERT_EQUAL(static_cast<int>(PhysicalQuantity::PRESSURE), static_cast<int>(r.quantity));
}

void test_tag_has_quantity() {
    TEST_ASSERT_EQUAL(static_cast<int>(PhysicalQuantity::PRESSURE), static_cast<int>(BoilerPressureTag::QUANTITY));
    TEST_ASSERT_EQUAL(static_cast<int>(PhysicalQuantity::TEMPERATURE), static_cast<int>(BoilerTempTag::QUANTITY));
}

void test_base_quantity_defaults_to_none() {
    Reading r;
    TEST_ASSERT_EQUAL(static_cast<int>(PhysicalQuantity::NONE), static_cast<int>(r.quantity));
}

void test_threshold_switch_rising() {
    SensorDispatcher registry;
    // Trigger at 1.1, release at 1.0 (Band = 0.1)
    ThresholdSwitch<BoilerPressureTag> trigger(&registry, 1.1f, 0.1f, ThresholdMode::ABOVE, "BAR");

    // 1. Below threshold
    registry.publish<BoilerPressureTag>(Reading(0.9f, "BAR", "", 1, false, PhysicalQuantity::PRESSURE));
    trigger.update();
    TEST_ASSERT_FALSE(trigger.isActive());

    // 2. Cross threshold -> Trigger
    registry.publish<BoilerPressureTag>(Reading(1.15f, "BAR", "", 1, false, PhysicalQuantity::PRESSURE));
    trigger.update();
    TEST_ASSERT_TRUE(trigger.isActive());
    TEST_ASSERT_TRUE(trigger.justStarted());

    // 3. Drop into band -> Stay Active (Hysteresis)
    registry.publish<BoilerPressureTag>(Reading(1.05f, "BAR", "", 1, false, PhysicalQuantity::PRESSURE));
    trigger.update();
    TEST_ASSERT_TRUE(trigger.isActive());

    // 4. Drop below reset -> Deactivate
    registry.publish<BoilerPressureTag>(Reading(0.95f, "BAR", "", 1, false, PhysicalQuantity::PRESSURE));
    trigger.update();
    TEST_ASSERT_FALSE(trigger.isActive());
    TEST_ASSERT_TRUE(trigger.justStopped());
}

void test_logical_and() {
    SensorDispatcher registry;
    auto t1 = new ThresholdSwitch<BoilerPressureTag>(&registry, 1.1f, 0.0f, ThresholdMode::ABOVE, "BAR");
    auto t2 = new ThresholdSwitch<BoilerTempTag>(&registry, 100.0f, 0.0f, ThresholdMode::ABOVE, "C");
    
    AndTrigger combined(t1, t2);

    // One true -> False
    registry.publish<BoilerPressureTag>(Reading(1.2f, "BAR", "", 1, false, PhysicalQuantity::PRESSURE));
    registry.publish<BoilerTempTag>(Reading(90.0f, "C", "", 0, false, PhysicalQuantity::TEMPERATURE));
    combined.update();
    TEST_ASSERT_FALSE(combined.isActive());

    // Both true -> True
    registry.publish<BoilerTempTag>(Reading(105.0f, "C", "", 0, false, PhysicalQuantity::TEMPERATURE));
    combined.update();
    TEST_ASSERT_TRUE(combined.isActive());
    TEST_ASSERT_TRUE(combined.justStarted());
}

void test_threshold_unit_mismatch() {
    SensorDispatcher registry;
    // Expect BAR, receive PSI
    ThresholdSwitch<BoilerPressureTag> trigger(&registry, 1.1f, 0.0f, ThresholdMode::ABOVE, "BAR");

    registry.publish<BoilerPressureTag>(Reading(1.5f, "PSI", "", 1, false, PhysicalQuantity::PRESSURE));
    trigger.update();
    
    // Should be false even though 1.5 > 1.1, because units Mismatch
    TEST_ASSERT_FALSE(trigger.isActive());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_reading_has_quantity);
    RUN_TEST(test_tag_has_quantity);
    RUN_TEST(test_base_quantity_defaults_to_none);
    RUN_TEST(test_threshold_switch_rising);
    RUN_TEST(test_logical_and);
    RUN_TEST(test_threshold_unit_mismatch);
    return UNITY_END();
}
