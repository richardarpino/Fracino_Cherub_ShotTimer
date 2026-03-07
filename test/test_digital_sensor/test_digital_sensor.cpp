#include <unity.h>
#include "Hardware/DigitalSensor.h"
#include "../../test/_common/stubs/SensorStub.h"
#include "../../test/_common/stubs/Arduino.cpp"

class MockSource : public IRawSource {
public:
    int val = HIGH;
    RawReading read() override { return RawReading(val, millis()); }
};

void test_digital_sensor_debounce() {
    MockSource source;
    // 50ms debounce
    DigitalSensor sensor(&source, true, 50);

    setHardwareTime(0);
    source.val = LOW; // Raw Active
    
    // Immediately after change
    Reading r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, r.value); // Should still be inactive
    
    setHardwareTime(25);
    r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, r.value); // Still inactive

    setHardwareTime(51);
    r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 1.0f, r.value); // Now active!

    // Noise: brief flip
    source.val = HIGH;
    setHardwareTime(60);
    r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 1.0f, r.value); // Should STAY active (filtered)

    setHardwareTime(111);
    r = sensor.getReading();
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, r.value); // Now inactive after debounce
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_digital_sensor_debounce);
    return UNITY_END();
}
