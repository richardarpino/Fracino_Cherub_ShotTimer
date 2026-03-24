#include <unity.h>
#include "Logic/Processors/ManualWeightProcessor.h"
#include "Logic/Triggers/ToggleTrigger.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"

void test_toggle_trigger_latches() {
    SensorDispatcher registry;
    // Mock switch that we can manually drive via registry
    RegistrySwitch<ButtonLeftReading> btn(&registry);
    ToggleTrigger trigger(&btn);

    // Initial state
    TEST_ASSERT_FALSE(trigger.isActive());

    // 1. Press Button (Publish 1.0)
    registry.publish<ButtonLeftReading>(1.0f);
    trigger.update();
    TEST_ASSERT_TRUE(trigger.isActive());

    // 2. Stay Pressed
    trigger.update();
    TEST_ASSERT_TRUE(trigger.isActive()); // Should stay latched

    // 3. Release Button
    registry.publish<ButtonLeftReading>(0.0f);
    trigger.update();
    TEST_ASSERT_TRUE(trigger.isActive()); // Should stay latched

    // 4. Press Again (Off)
    registry.publish<ButtonLeftReading>(1.0f);
    trigger.update();
    TEST_ASSERT_FALSE(trigger.isActive()); // Should toggle OFF
}

void test_manual_weight_processor_reacts() {
    SensorDispatcher registry;
    ManualWeightProcessor proc(&registry);
    registry.attachProcessor<ManualWeightReading>(&proc);

    // 1. Initial State
    registry.publish<WeightReading>(50.0f); // Some initial weight on scale
    proc.update();
    TEST_ASSERT_EQUAL_FLOAT(50.0f, registry.getLatest<ManualWeightReading>().value);

    // 2. Tare
    proc.tare();
    proc.update();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, registry.getLatest<ManualWeightReading>().value);

    // 3. Add 10g
    registry.publish<WeightReading>(60.0f); // This triggers proc automatically via attachProcessor
    // In native test, we might need to manually call update if we don't use the dispatcher loop correctly,
    // but publish() calls triggerResolution() which calls update().
    TEST_ASSERT_EQUAL_FLOAT(10.0f, registry.getLatest<ManualWeightReading>().value);
}

void test_toggle_with_double_update() {
    SensorDispatcher registry;
    RegistrySwitch<ButtonLeftReading> btn(&registry);
    ToggleTrigger trigger(&btn);

    // 1. Press Button (Publish 1.0)
    registry.publish<ButtonLeftReading>(1.0f);
    
    // Simulate main loop (update twice)
    trigger.update(); // Call 1 (Heartbeat)
    TEST_ASSERT_TRUE(trigger.isActive());
    
    trigger.update(); // Call 2 (Loop)
    TEST_ASSERT_TRUE(trigger.isActive()); // Should STILL be active

    // 2. Release Button
    registry.publish<ButtonLeftReading>(0.0f);
    trigger.update(); 
    trigger.update();
    TEST_ASSERT_TRUE(trigger.isActive()); // Should stay latched

    // 3. Press Again to toggle OFF
    registry.publish<ButtonLeftReading>(1.0f);
    trigger.update(); // Call 1 (Heartbeat) - This should toggle it OFF
    TEST_ASSERT_FALSE(trigger.isActive());
    
    trigger.update(); // Call 2 (Loop)
    TEST_ASSERT_FALSE(trigger.isActive());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_toggle_trigger_latches);
    RUN_TEST(test_manual_weight_processor_reacts);
    RUN_TEST(test_toggle_with_double_update);
    return UNITY_END();
}
