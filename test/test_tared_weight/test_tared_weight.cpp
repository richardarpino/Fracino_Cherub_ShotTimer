#include <unity.h>
#include "Logic/Processors/TaredWeightProcessor.h"
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

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_tared_weight_locking_rigor);
    return UNITY_END();
}
