#include <unity.h>
#include "SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "SensorTags.h"
#include "../_common/stubs/SensorStub.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

void test_dispatcher_provides_latest_reading() {
    SensorDispatcher dispatcher;
    SensorStub pressureSensor;
    
    pressureSensor.setReading(Reading(1.5f, "bar", "PRESSURE", 1, false));
    
    // Wire the concrete sensor to the logical Tag
    dispatcher.provide<BoilerPressureTag>(&pressureSensor);
    
    dispatcher.update();
    
    Reading r = dispatcher.getLatest<BoilerPressureTag>();
    
    TEST_ASSERT_EQUAL_FLOAT(1.5f, r.value);
    TEST_ASSERT_EQUAL_STRING("bar", r.unit.c_str());
    TEST_ASSERT_FALSE(r.isError);
}

void test_dispatcher_ensures_synchronization() {
    SensorDispatcher dispatcher;
    SensorStub pressureSensor;
    
    dispatcher.provide<BoilerPressureTag>(&pressureSensor);
    
    // First update
    pressureSensor.setReading(Reading(1.0f, "bar", "PRESSURE", 1, false));
    dispatcher.update();
    
    Reading r1 = dispatcher.getLatest<BoilerPressureTag>();
    
    // Change sensor value WITHOUT calling dispatcher.update()
    pressureSensor.setReading(Reading(2.0f, "bar", "PRESSURE", 1, false));
    
    Reading r2 = dispatcher.getLatest<BoilerPressureTag>();
    
    // Both should still be the "frozen" value (1.0f) from the last update() call
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r1.value);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r2.value);
}

void test_dispatcher_unregistered_tag_returns_invalid() {
    SensorDispatcher dispatcher;
    
    // We haven't registered WeightTag
    Reading r = dispatcher.getLatest<WeightTag>();
    
    TEST_ASSERT_TRUE(r.isError);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dispatcher_provides_latest_reading);
    RUN_TEST(test_dispatcher_ensures_synchronization);
    RUN_TEST(test_dispatcher_unregistered_tag_returns_invalid);
    return UNITY_END();
}
