#include <unity.h>
#include "Hardware/BoilerPressure.h"
#include "../_common/MockRawSource.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Utils/StringUtils.h"

void test_pressure_rounding_jitter() {
    MockRawSource mock;
    BoilerPressure pressure(&mock);

    // 1. Settle just ABOVE a rounding boundary (1.16 Bar -> rounds to 1.2)
    // 1.16 Bar -> (1.16/1.25 + 0.5) = 1.428V sensor -> 0.714V pin -> (0.714/3.3)*4095 = 886 raw
    mock.setRawValue(886); 
    for(int i=0; i<100; i++) { setMillis(i); pressure.getReading(); }
    
    float v1 = pressure.getReading().value;
    char buf1[16];
    StringUtils::formatFloat1(buf1, sizeof(buf1), v1);
    printf("DEBUG: V1=%.3f, Display=%s\n", v1, buf1);

    // 2. Small move (0.025 Bar) that triggers hysteresis and crosses boundary (1.135 Bar -> rounds to 1.1)
    // 1.135 Bar -> (1.135/1.25 + 0.5) = 1.408V sensor -> 0.704V pin -> (0.704/3.3)*4095 = 873 raw
    mock.setRawValue(873); 
    for(int i=100; i<200; i++) { setMillis(i); pressure.getReading(); }
    
    float v2 = pressure.getReading().value;
    char buf2[16];
    StringUtils::formatFloat1(buf2, sizeof(buf2), v2);
    printf("DEBUG: V2=%.3f, Display=%s\n", v2, buf2);

    // 3. Move back UP by 0.021 Bar
    mock.setRawValue(896); 
    for(int i=200; i<300; i++) { setMillis(i); pressure.getReading(); }
    
    float v3 = pressure.getReading().value;
    char buf3[16];
    StringUtils::formatFloat1(buf3, sizeof(buf3), v3);
    printf("DEBUG: V3=%.3f, Display=%s\n", v3, buf3);

    // PROOF: With the fix (0.05 Bar hysteresis), a move of 0.021 Bar 
    // should NOT flip the display anymore, even near the rounding boundary.
    TEST_ASSERT_EQUAL_STRING_MESSAGE(buf1, buf2, "Display should be stable now with 0.05 hysteresis");
    TEST_ASSERT_EQUAL_STRING(buf1, buf3); // Long term stability
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_rounding_jitter);
    return UNITY_END();
}
