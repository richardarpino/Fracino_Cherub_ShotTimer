#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Interfaces/PhysicalUnits.h"

void test_unit_formatting_pressure() {
    char buf[16];
    
    // 1.25 bar should round/format to "1.3bar" (using StringUtils::formatFloat1)
    Reading r1 = Units::Pressure.make(1.25f, "TEST");
    r1.format(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("1.3bar", buf);
    
    // 0.0 bar
    Reading r2 = Units::Pressure.make(0.0f, "TEST");
    r2.format(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("0.0bar", buf);
}

void test_unit_formatting_temperature() {
    char buf[16];
    
    // Temperature has 0 precision
    Reading r1 = Units::Temperature.make(95.6f, "TEST");
    r1.format(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("96C", buf); // Rounded to nearest int
}

void test_unit_formatting_error() {
    char buf[16];
    
    Reading r = Units::Pressure.make(1.2f, "TEST", true);
    r.format(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("ERR", buf);
}

void test_unit_dsl_metadata() {
    SensorMetadata meta = Units::Pressure.range("BOILER", 0.0f, 3.0f);
    
    TEST_ASSERT_EQUAL_FLOAT(0.0f, meta.low.value);
    TEST_ASSERT_EQUAL_STRING("bar", meta.low.unit);
    TEST_ASSERT_EQUAL_STRING("BOILER", meta.low.label);
    TEST_ASSERT_EQUAL(1, meta.low.precision);
    
    TEST_ASSERT_EQUAL_FLOAT(3.0f, meta.high.value);
    TEST_ASSERT_TRUE(meta.error.isError);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_unit_formatting_pressure);
    RUN_TEST(test_unit_formatting_temperature);
    RUN_TEST(test_unit_formatting_error);
    RUN_TEST(test_unit_dsl_metadata);
    return UNITY_END();
}
