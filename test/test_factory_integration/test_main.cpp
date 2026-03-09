#include <unity.h>
#include <lvgl.h>
#include "../../lib/Factories/LVGLWidgetFactory.h"
#include "../../lib/Registry/WidgetRegistry.h"
#include "../../lib/Registry/WidgetTags.h"
#include "../../lib/Interfaces/SensorTags.h"

unsigned long millis() { return 0; }

// Mock for Simulator context
class MockSensorRegistry : public ISensorRegistry {
public:
    void update() override {}
    bool hasProcessor(const char* name) override { return false; }
    void triggerResolution(const char* name) override {}
    Reading getReadingByName(const char* name) override {
        if (strcmp(name, BoilerPressureReading::NAME) == 0) return Reading(0.0f, "BAR", "PRESS", 1, false, PhysicalQuantity::PRESSURE);
        return Reading(0.0f, "", "", 0, true);
    }
    void setReadingByName(const char* name, Reading reading) override {}
    StatusMessage getStatusByName(const char* name) override { return StatusMessage(); }
    void setStatusByName(const char* name, StatusMessage status) override {}
    void attachProcessorInternal(const char* targetTagName, class ITagProcessor* processor) override {}
    DataCategory getCategory(const char* name) override { return DataCategory::TELEMETRY; }
    PhysicalQuantity getQuantity(const char* name) override {
        if (strcmp(name, BoilerPressureReading::NAME) == 0) return PhysicalQuantity::PRESSURE;
        return PhysicalQuantity::NONE;
    }
};

void test_factory_creates_compatible_widget() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);
    LVGLWidgetFactory factory(&widgetRegistry);

    // We expect to be able to set the registry later, or it's passed in
    // For now, let's assume our new interface: createWidget(widgetName, tagName, registry)
    
    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::PRESSURE }
    ));

    // This SHOULD compile after we update IWidgetFactory
    // For RED phase, we use the name from the Tag
    IWidget* widget = factory.createWidget(GaugeWidgetTag::NAME, BoilerPressureReading::NAME, &sensorRegistry);
    
    TEST_ASSERT_NOT_NULL(widget);
    delete widget;
}

void test_factory_rejects_incompatible_widget() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);
    LVGLWidgetFactory factory(&widgetRegistry);

    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::TEMPERATURE } // Only TEMPERATURE
    ));

    // BoilerPressure is PRESSURE
    IWidget* widget = factory.createWidget(GaugeWidgetTag::NAME, BoilerPressureReading::NAME, &sensorRegistry);
    
    TEST_ASSERT_NULL(widget);
}

// In a real simulator test, we'd have a main or setup
// For now, we'll let the PIO test runner handle it if we put it in the right place.
// Since it's a "simulator" test, it usually needs LVGL init etc.
// But we are testing factory logic which is mostly setup-independent.

void setUp(void) {
    // LVGL Init if needed
}

void tearDown(void) {
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_factory_creates_compatible_widget);
    RUN_TEST(test_factory_rejects_incompatible_widget);
    return UNITY_END();
}
