#include <unity.h>
#include "Logic/Processors/TaredWeightProcessor.h"
#include "Logic/Processors/ManualWeightProcessor.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"

void test_tared_weight_locking_rigor() {
    SensorDispatcher registry;
    TaredWeightProcessor proc(&registry);

    // Initial State: Idle
    registry.publish<WeightReading>(100.0f);
    proc.update();
    Reading output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(100.0f, output.value);

    // 1. Pump Starts -> Tare should be captured
    registry.publish<PumpReading>(1.0f);
    proc.update();
    output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, output.value); // (100 - 100)

    // 2. Weight Increases
    registry.publish<WeightReading>(110.0f);
    proc.update();
    output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(10.0f, output.value); // (110 - 100)

    // 3. Pump Stops -> Weight should LOCK
    registry.publish<PumpReading>(0.0f);
    proc.update();
    output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(10.0f, output.value); // Still 10

    // 4. Noise/Solenoid Purge (Weight increases)
    registry.publish<WeightReading>(115.0f);
    proc.update();
    output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(10.0f, output.value); // SHOULD REMAIN 10 (LOCKED)

    // 5. Cup Removed (Weight drops)
    registry.publish<WeightReading>(20.0f);
    proc.update();
    output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(10.0f, output.value); // SHOULD REMAIN 10 (LOCKED)

    // 6. Next Shot Starts -> UNLOCK and RE-TARE
    registry.publish<PumpReading>(1.0f);
    proc.update(); 
    // New offset should be 20.0 (current weight)
    // Tared yield starts at 0.0
    output = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, output.value);
}

void test_americano_scenario() {
    SensorDispatcher registry;
    TaredWeightProcessor shotProc(&registry);
    ManualWeightProcessor manualProc(&registry);

    // 1. Initial State: Scale empty
    registry.publish<WeightReading>(0.0f);
    shotProc.update();
    manualProc.update();
    
    // 2. Enter Scale Mode (Button Press) -> Manual Tare to 0
    manualProc.tare(); 
    
    // 3. Add 100g Hot Water
    registry.publish<WeightReading>(100.0f);
    shotProc.update();
    manualProc.update();
    
    // ManualWeight should be 100.0
    Reading manual = registry.getLatest<ManualWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(100.0f, manual.value);

    // 4. Start Pump (Espresso Shot)
    registry.publish<PumpReading>(1.0f);
    shotProc.update();
    manualProc.update();
    
    // Shot Timer should show 0.0g (Fresh tare for the shot)
    Reading shot = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(0.0f, shot.value);

    // 5. Add 30g Espresso
    registry.publish<WeightReading>(130.0f);
    shotProc.update();
    manualProc.update();
    
    shot = registry.getLatest<TaredWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(30.0f, shot.value);
    
    // 6. Manual Weight should still show CUMULATIVE (130.0)
    manual = registry.getLatest<ManualWeightReading>();
    TEST_ASSERT_EQUAL_FLOAT(130.0f, manual.value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_tared_weight_locking_rigor);
    RUN_TEST(test_americano_scenario);
    return UNITY_END();
}
