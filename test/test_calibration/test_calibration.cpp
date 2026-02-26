#include <unity.h>
#include "Hardware/BoilerPressure.h"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "pins.h"

void test_pressure_scalar_application() {
    MockRawSource mock;
    
    // We expect the scalar from pins.h to be passed to BoilerPressure in MachineFactory.
    // Here we test the class behavior with the current scalar.
    BoilerPressure sensor(&mock, pressureScalar);
    
    // 1.0 Bar baseline (ADC 806 -> 1.0 Bar with scalar 1.0)
    mock.setRawValue(806);
    for(int i=0; i<100; i++) { setMillis(i); sensor.getReading(); }
    
    float rawExpected = 1.0f;
    float expectedValue = rawExpected * pressureScalar;
    
    TEST_ASSERT_FLOAT_WITHIN(0.05f, expectedValue, sensor.getReading().value);
}

void test_pressure_scalar_1_25() {
    MockRawSource mock;
    // Explicitly test that 1.25 behaves as expected for the user's scenario
    BoilerPressure sensor(&mock, 1.25f);
    
    mock.setRawValue(806); // 1.0 Bar raw
    for(int i=0; i<100; i++) { setMillis(i); sensor.getReading(); }
    
    TEST_ASSERT_FLOAT_WITHIN(0.05f, 1.25f, sensor.getReading().value);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_scalar_application);
    RUN_TEST(test_pressure_scalar_1_25);
    return UNITY_END();
}
