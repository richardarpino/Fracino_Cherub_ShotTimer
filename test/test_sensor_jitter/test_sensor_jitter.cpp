#include <unity.h>
#include "Hardware/BoilerPressure.h"
#include "Virtual/BoilerTemperature.h"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_temperature_jitter_reproduction() {
    MockRawSource mock;
    BoilerPressure pressure(&mock);
    BoilerTemperature temp(&pressure);

    // Initial settle
    mock.setRawValue(806); // ~1.0 Bar -> ~120.5C
    for(int i=0; i<100; i++) { setMillis(i); pressure.getReading(); }
    
    float startTemp = temp.getReading().value;
    printf("DEBUG: Started at %.3f C\n", startTemp);

    // Step 1: Push pressure UP by 0.021 Bar to trigger hysteresis
    // (Approx 10 raw units)
    mock.setRawValue(817); 
    for(int i=100; i<200; i++) { setMillis(i); pressure.getReading(); }
    float highTemp = temp.getReading().value;
    
    // Step 2: Push back DOWN by 0.021 Bar
    mock.setRawValue(806);
    for(int i=200; i<300; i++) { setMillis(i); pressure.getReading(); }
    float lowTemp = temp.getReading().value;

    printf("DEBUG: HighTemp=%.3f, LowTemp=%.3f, Delta=%.3f\n", highTemp, lowTemp, highTemp - lowTemp);
    
    // VERIFICATION: With the fix (independent filtering for temp + 0.5C hysteresis),
    // the value should stay stable even if pressure moves slightly.
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, highTemp - lowTemp, "Temperature should be stable despite pressure noise");
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_jitter_reproduction);
    return UNITY_END();
}
