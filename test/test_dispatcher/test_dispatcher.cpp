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

// --- Widget-Registry Integration Tests ---

template<typename Tag>
class MockRegistryWidget {
public:
    MockRegistryWidget(ISensorRegistry* registry) : _registry(registry) {}
    
    Reading getLatest() {
        return _registry->getLatest<Tag>();
    }

private:
    ISensorRegistry* _registry;
};

void test_widget_logic_pulls_correct_tag() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    SensorStub tempSensor;

    pressureSensor.setReading(Reading(1.5f, "bar", "P", 1, false));
    tempSensor.setReading(Reading(95.0f, "C", "T", 1, false));

    registry.provide<BoilerPressureTag>(&pressureSensor);
    registry.provide<BoilerTempTag>(&tempSensor);
    registry.update();

    MockRegistryWidget<BoilerPressureTag> pressureWidget(&registry);
    MockRegistryWidget<BoilerTempTag> tempWidget(&registry);

    TEST_ASSERT_EQUAL_FLOAT(1.5f, pressureWidget.getLatest().value);
    TEST_ASSERT_EQUAL_FLOAT(95.0f, tempWidget.getLatest().value);
}

void test_widget_logic_reflects_registry_sync() {
    SensorDispatcher registry;
    SensorStub sensor;
    registry.provide<BoilerPressureTag>(&sensor);

    MockRegistryWidget<BoilerPressureTag> widget(&registry);

    sensor.setReading(Reading(1.0f, "bar", "P", 1, false));
    registry.update();
    Reading r1 = widget.getLatest();

    sensor.setReading(Reading(2.0f, "bar", "P", 1, false));
    // Notice: NO registry.update() here
    Reading r2 = widget.getLatest();

    TEST_ASSERT_EQUAL_FLOAT(1.0f, r1.value);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r2.value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dispatcher_provides_latest_reading);
    RUN_TEST(test_dispatcher_ensures_synchronization);
    RUN_TEST(test_dispatcher_unregistered_tag_returns_invalid);
    RUN_TEST(test_widget_logic_pulls_correct_tag);
    RUN_TEST(test_widget_logic_reflects_registry_sync);
    return UNITY_END();
}
