#include <unity.h>
#include <type_traits>
#include "SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "SensorTags.h"
#include "../_common/stubs/SensorStub.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

// RED: These checks will fail to compile because tags don't inherit from these yet
// and the base classes themselves don't exist.
// RED: These checks will fail to compile because tags don't inherit from these yet
// and the base classes themselves don't exist.
static_assert(std::is_base_of_v<BaseTelemetryTag, BoilerPressureReading>, "Must be TelemetryTag");
static_assert(std::is_base_of_v<BaseServiceTag, WiFiStatus>, "Must be ServiceTag");

void test_dispatcher_provides_latest_reading() {
    SensorDispatcher dispatcher;
    SensorStub pressureSensor;
    
    pressureSensor.setReading(1.5f);
    
    // Wire the concrete sensor to the logical Tag
    dispatcher.provide<BoilerPressureReading>(&pressureSensor);
    
    dispatcher.update();
    
    Reading r = dispatcher.getLatest<BoilerPressureReading>();
    
    TEST_ASSERT_EQUAL_FLOAT(1.5f, r.value);
    TEST_ASSERT_EQUAL_STRING("bar", r.unit);
    TEST_ASSERT_FALSE(r.isError);
}

void test_dispatcher_ensures_synchronization() {
    SensorDispatcher dispatcher;
    SensorStub pressureSensor;
    
    dispatcher.provide<BoilerPressureReading>(&pressureSensor);
    
    // First update
    pressureSensor.setReading(1.0f);
    dispatcher.update();
    
    Reading r1 = dispatcher.getLatest<BoilerPressureReading>();
    
    // Change sensor value WITHOUT calling dispatcher.update()
    pressureSensor.setReading(2.0f);
    
    Reading r2 = dispatcher.getLatest<BoilerPressureReading>();
    
    // Both should still be the "frozen" value (1.0f) from the last update() call
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r1.value);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r2.value);
}

void test_dispatcher_unregistered_tag_returns_invalid() {
    SensorDispatcher dispatcher;
    
    // We haven't registered WeightReading, so it returns its DSL-defined init value
    Reading r = dispatcher.getLatest<WeightReading>();
    
    TEST_ASSERT_EQUAL_FLOAT(0.0f, r.value);
    TEST_ASSERT_EQUAL_STRING("g", r.unit);
    TEST_ASSERT_FALSE(r.isError);
}

void test_dispatcher_provides_latest_status() {
    SensorDispatcher dispatcher;
    dispatcher.publish<WiFiStatus>(StatusMessage("WiFi", "CONNECTED", 100.0f, false));
    
    // RED: This should return StatusMessage in the future
    StatusMessage s = dispatcher.getLatest<WiFiStatus>();
    
    TEST_ASSERT_EQUAL_STRING("CONNECTED", s.message);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, s.progress);
}

void test_dispatcher_raw_float_publication() {
    SensorDispatcher dispatcher;
    
    // Publish raw float for BoilerPressureReading (metadata unit: bar, precision: 1)
    dispatcher.publish<BoilerPressureReading>(1.23f);
    
    Reading r = dispatcher.getLatest<BoilerPressureReading>();
    TEST_ASSERT_EQUAL_FLOAT(1.23f, r.value);
    TEST_ASSERT_EQUAL_STRING("bar", r.unit);
    TEST_ASSERT_EQUAL(1, r.precision);
    
    // Verify formatting follows metadata precision
    char buf[16];
    r.format(buf, sizeof(buf));
    TEST_ASSERT_EQUAL_STRING("1.2bar", buf); // Rounded/formatted to 1 decimal
}

// --- Widget-Registry Integration Tests ---

template<typename Tag>
class MockRegistryWidget {
public:
    MockRegistryWidget(ISensorRegistry* registry = nullptr) : _registry(registry) {}
    
    void setRegistry(ISensorRegistry* registry) {
        _registry = registry;
    }
    
    Reading getLatest() {
        if (!_registry) return Reading(0, "", "ERROR", 0, true);
        return _registry->getLatest<Tag>();
    }

private:
    ISensorRegistry* _registry;
};

void test_widget_logic_pulls_correct_tag() {
    SensorDispatcher registry;
    SensorStub pressureSensor;
    SensorStub tempSensor;

    pressureSensor.setReading(1.5f);
    tempSensor.setReading(95.0f);

    registry.provide<BoilerPressureReading>(&pressureSensor);
    registry.provide<BoilerTempReading>(&tempSensor);
    registry.update();

    MockRegistryWidget<BoilerPressureReading> pressureWidget(&registry);
    MockRegistryWidget<BoilerTempReading> tempWidget(&registry);

    TEST_ASSERT_EQUAL_FLOAT(1.5f, pressureWidget.getLatest().value);
    TEST_ASSERT_EQUAL_FLOAT(95.0f, tempWidget.getLatest().value);
}

void test_widget_logic_reflects_registry_sync() {
    SensorDispatcher registry;
    SensorStub sensor;
    registry.provide<BoilerPressureReading>(&sensor);

    MockRegistryWidget<BoilerPressureReading> widget(&registry);

    sensor.setReading(1.0f);
    registry.update();
    Reading r1 = widget.getLatest();

    sensor.setReading(2.0f);
    // Notice: NO registry.update() here
    Reading r2 = widget.getLatest();

    TEST_ASSERT_EQUAL_FLOAT(1.0f, r1.value);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, r2.value);
}

void test_widget_logic_supports_late_binding() {
    SensorDispatcher registry;
    SensorStub sensor;
    sensor.setReading(3.14f);
    registry.provide<BoilerPressureReading>(&sensor);
    registry.update();

    // Construct WITHOUT registry
    MockRegistryWidget<BoilerPressureReading> widget;
    TEST_ASSERT_TRUE(widget.getLatest().isError);

    // Late Binding
    widget.setRegistry(&registry);
    TEST_ASSERT_EQUAL_FLOAT(3.14f, widget.getLatest().value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_dispatcher_provides_latest_reading);
    RUN_TEST(test_dispatcher_ensures_synchronization);
    RUN_TEST(test_dispatcher_unregistered_tag_returns_invalid);
    RUN_TEST(test_dispatcher_provides_latest_status);
    RUN_TEST(test_dispatcher_raw_float_publication);
    RUN_TEST(test_widget_logic_pulls_correct_tag);
    RUN_TEST(test_widget_logic_reflects_registry_sync);
    RUN_TEST(test_widget_logic_supports_late_binding);
    return UNITY_END();
}
