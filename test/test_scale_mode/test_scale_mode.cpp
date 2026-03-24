#include <unity.h>
#include "Logic/Processors/ManualWeightProcessor.h"
#include "Logic/Processors/WeightCalibrationProcessor.h"
#include "Sensors/Hardware/WeightSensor.h"
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

void test_weight_calibration_ema_smoothing() {
    SensorDispatcher registry;
    
    // 1. Hardware Layer: Setup raw source mock and sensor
    class MockSource : public IRawSource {
    public:
        MockSource() : val(0), ts(0) {}
        RawReading read() override { return RawReading(val, ++ts); }
        int val;
        unsigned long ts;
    } source;

    // Create hardware sensor with EMA alpha = 0.5
    WeightSensor hw(&source, 0.5f);
    registry.provide<RawWeightReading>(&hw);

    // 2. Logic Layer: Setup calibration processor
    WeightCalibrationProcessor proc(&registry, 10.0f, 1.0f); // Offset 10, Multiplier 1
    registry.attachProcessor<WeightReading>(&proc);

    // TEST CASE 1: Initial state - Snap to value
    source.val = 110; 
    registry.update(); // Polls hw -> publishes RawWeight=110 -> triggers proc -> publishes Weight=100
    TEST_ASSERT_EQUAL_FLOAT(100.0f, registry.getLatest<WeightReading>().value);

    // TEST CASE 2: Smoothing - (0.5 * 210) + (0.5 * 110) = 160 RAW -> (160 - 10) = 150 CALIBRATED
    source.val = 210;
    registry.update();
    TEST_ASSERT_EQUAL_FLOAT(150.0f, registry.getLatest<WeightReading>().value);
}

void test_weight_sensor_adaptive_alpha() {
    SensorDispatcher registry;
    class MockSource : public IRawSource {
    public:
        MockSource() : val(0), ts(0) {}
        RawReading read() override { return RawReading(val, ++ts); }
        int val;
        unsigned long ts;
    } source;

    // Use alpha = 0.05 (Slow) for small changes
    WeightSensor hw(&source, 0.05f, 0.0f); 

    // 1. Initial Reading - snap
    source.val = 1000;
    TEST_ASSERT_EQUAL_FLOAT(1000.0f, hw.getReading());

    // 2. Large Change (1000 -> 2000) - Should reach 1800 in 1 step (Alpha=0.8)
    // Diff is 1000, which is > 100 (threshold)
    source.val = 2000;
    TEST_ASSERT_EQUAL_FLOAT(1800.0f, hw.getReading());

    // 3. Small Change (1800 -> 1810) - Should use Alpha=0.05
    // Diff is 10, which is < 100
    // Result: (0.05 * 1810) + (0.95 * 1800) = 90.5 + 1710 = 1800.5
    source.val = 1810;
    TEST_ASSERT_EQUAL_FLOAT(1800.5f, hw.getReading());
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
    RUN_TEST(test_weight_calibration_ema_smoothing);
    RUN_TEST(test_weight_sensor_adaptive_alpha);
    RUN_TEST(test_toggle_with_double_update);
    return UNITY_END();
}
