#include <unity.h>
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "SensorTypes.h"
#include "../../lib/Hardware/HX711Source.h"

void test_hx711_returns_cached_value_when_not_ready() {
    resetArduinoStubs();
    HX711Source source(39, 33);
    
    // Initial state: lastValue = 0
    setDigitalRead(39, HIGH); // Not ready
    RawReading r1 = source.read();
    TEST_ASSERT_EQUAL(0, r1.value);

    // Simulate a previous value being stored
    setDigitalRead(39, LOW); // Ready
    int bits_zero[24] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
    setDigitalReadSequence(39, bits_zero, 24);
    source.read(); // Now cached 0

    setDigitalRead(39, HIGH); // Not ready again
    RawReading r2 = source.read();
    TEST_ASSERT_EQUAL(0, r2.value);
}

void test_hx711_reads_positive_24_bit_value() {
    resetArduinoStubs();
    HX711Source source(39, 33);
    
    // Sequence starts with a LOW for the initial ready check, then 24 data bits.
    int bits[25] = {
        0, // Ready signal
        0,0,0,1, 0,0,1,0, 0,0,1,1, 0,1,0,0, 0,1,0,1, 0,1,1,0
    };
    
    setDigitalReadSequence(39, bits, 25);
    
    RawReading r = source.read();
    TEST_ASSERT_EQUAL(0x123456, r.value);
}

void test_hx711_reads_negative_24_bit_value() {
    resetArduinoStubs();
    HX711Source source(39, 33);
    
    // Sequence starts with a LOW for the initial ready check, then 24 data bits.
    int bits[25] = {
        0, // Ready signal
        1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1
    };
    
    setDigitalReadSequence(39, bits, 25);
    
    RawReading r = source.read();
    TEST_ASSERT_EQUAL(-1, r.value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_hx711_returns_cached_value_when_not_ready);
    RUN_TEST(test_hx711_reads_positive_24_bit_value);
    RUN_TEST(test_hx711_reads_negative_24_bit_value);
    return UNITY_END();
}
