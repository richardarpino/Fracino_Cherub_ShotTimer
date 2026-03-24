#include <unity.h>
#include <cmath>
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Sensors/Hardware/WeightSensor.h"

// Mock source to simulate HX711 data arrival
class MockRawSource : public IRawSource {
public:
    MockRawSource() : _val(0), _ts(0) {}
    void push(int val) { _val = val; _ts++; }
    RawReading read() override { return RawReading(_val, _ts); }
private:
    int _val;
    uint32_t _ts;
};

void test_weight_settlement_speed() {
    resetArduinoStubs();
    MockRawSource mock;
    WeightSensor sensor(&mock, 0.5f, 30.0f); // Fast alpha, 30 count hysteresis
    
    // Simulate sitting at 120g (3600 counts)
    mock.push(3600);
    float val = 0;
    for (int i = 0; i < 50; i++) {
        mock.push(3600);
        val = sensor.getReading();
    }
    TEST_ASSERT_EQUAL_FLOAT(3600.0f, val);

    // Abrupt drop to 0.0g (0 counts)
    int samples = 0;
    while (val > 1.0f && samples < 20) {
        mock.push(0);
        val = sensor.getReading();
        samples++;
    }
    
    // 0.7s at 10Hz = 7 samples. Success if settled in <= 10 samples.
    TEST_ASSERT_LESS_THAN(11, samples); 
    TEST_ASSERT_EQUAL_FLOAT(0.0f, val);
}

void test_zero_snap_persistence() {
    resetArduinoStubs();
    MockRawSource mock;
    WeightSensor sensor(&mock, 0.5f, 30.0f);
    
    // Simulate a residual float within hysteresis (e.g., 20 counts = 0.66g)
    mock.push(20);
    float val = sensor.getReading();
    
    // Zero-Snap should force this to 0.0
    TEST_ASSERT_EQUAL_FLOAT(0.0f, val);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_weight_settlement_speed);
    RUN_TEST(test_zero_snap_persistence);
    return UNITY_END();
}
