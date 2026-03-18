#include <unity.h>
#include <cstring>
#include "../../lib/Registry/WidgetRegistry.h"
#include "../../lib/Registry/WidgetTags.h"
#include "../../lib/Interfaces/SensorTags.h"

class MockSensorRegistry : public ISensorRegistry {
public:
    void update() override {}
    bool hasProcessor(const char* name) override { return false; }
    void triggerResolution(const char* name) override {}
    
    DataCategory getCategory(const char* name) override {
        if (strcmp(name, WiFiStatus::NAME) == 0) return DataCategory::SERVICE;
        return DataCategory::TELEMETRY;
    }

    PhysicalQuantity getQuantity(const char* name) override {
        if (strcmp(name, BoilerPressureReading::NAME) == 0) return PhysicalQuantity::PRESSURE;
        if (strcmp(name, WeightReading::NAME) == 0) return PhysicalQuantity::WEIGHT;
        if (strcmp(name, ShotTimeReading::NAME) == 0) return PhysicalQuantity::TIME;
        return PhysicalQuantity::NONE;
    }

    SensorMetadata getSensorMetadataByName(const char* name) override { return SensorMetadata(); }
    ServiceMetadata getServiceMetadataByName(const char* name) override { return ServiceMetadata(); }

    Reading getReadingByName(const char* name) override {
        if (strcmp(name, BoilerPressureReading::NAME) == 0) return Reading(0.0f, "BAR", "BOILER", 1, false, PhysicalQuantity::PRESSURE);
        if (strcmp(name, WeightReading::NAME) == 0) return Reading(0.0f, "G", "WEIGHT", 1, false, PhysicalQuantity::WEIGHT);
        if (strcmp(name, ShotTimeReading::NAME) == 0) return Reading(0.0f, "S", "SHOT", 1, false, PhysicalQuantity::TIME);
        return Reading(0.0f, "", "", 0, true); // Not found
    }
    void setReadingByName(const char* name, Reading reading) override {}
    StatusMessage getStatusByName(const char* name) override {
        if (strcmp(name, WiFiStatus::NAME) == 0) return StatusMessage("WiFi", "OK", 100.0f, false);
        return StatusMessage("", "", 0.0f, true); // Not found
    }
    void setStatusByName(const char* name, StatusMessage status) override {}
    
    void storeMetadataInternal(const char* name, SensorMetadata meta) override {}
    void storeMetadataInternal(const char* name, ServiceMetadata meta) override {}
    
    void attachProcessorInternal(const char* targetTagName, ITagProcessor* processor) override {}
};

void test_categorical_match_telemetry() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::PRESSURE, PhysicalQuantity::TEMPERATURE }
    ));

    TEST_ASSERT_TRUE(widgetRegistry.isCompatible(GaugeWidgetTag::NAME, BoilerPressureReading::NAME));
}

void test_specific_tag_match_telemetry() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    widgetRegistry.registerWidget<ShotTimerWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        {},
        { ShotTimeReading::NAME }
    ));

    TEST_ASSERT_TRUE(widgetRegistry.isCompatible(ShotTimerWidgetTag::NAME, ShotTimeReading::NAME));
}

void test_precedence_specific_over_categorical() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    // Register a widget that ONLY allows a specific tag, even if quantities match
    widgetRegistry.registerWidget<ShotTimerWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::TIME },
        { ShotTimeReading::NAME }
    ));

    TEST_ASSERT_TRUE(widgetRegistry.isCompatible(ShotTimerWidgetTag::NAME, ShotTimeReading::NAME));
}

void test_unhappy_quantity_mismatch() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::PRESSURE }
    ));

    TEST_ASSERT_FALSE(widgetRegistry.isCompatible(GaugeWidgetTag::NAME, WeightReading::NAME));
}

void test_unhappy_category_mismatch() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::PRESSURE }
    ));

    // WiFi is a SERVICE
    TEST_ASSERT_FALSE(widgetRegistry.isCompatible(GaugeWidgetTag::NAME, WiFiStatus::NAME));
}

void test_unhappy_missing_tag() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::PRESSURE }
    ));

    TEST_ASSERT_FALSE(widgetRegistry.isCompatible(GaugeWidgetTag::NAME, "NonExistentTag"));
}

void test_unhappy_unregistered_widget() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    TEST_ASSERT_FALSE(widgetRegistry.isCompatible("UnknownWidget", BoilerPressureReading::NAME));
}

void test_type_based_whitelisting() {
    MockSensorRegistry sensorRegistry;
    WidgetRegistry widgetRegistry(&sensorRegistry);

    // Whitelist only BoilerPressure
    widgetRegistry.applyTypeWhitelists(TagList<BoilerPressureReading>{}, TagList<>{});

    widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(DataCategory::TELEMETRY));

    // BoilerPressure should be allowed
    TEST_ASSERT_TRUE(widgetRegistry.isCompatible(GaugeWidgetTag::NAME, BoilerPressureReading::NAME));
    // Weight should NOT be allowed (not in whitelist)
    TEST_ASSERT_FALSE(widgetRegistry.isCompatible(GaugeWidgetTag::NAME, WeightReading::NAME));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_categorical_match_telemetry);
    RUN_TEST(test_specific_tag_match_telemetry);
    RUN_TEST(test_precedence_specific_over_categorical);
    RUN_TEST(test_unhappy_quantity_mismatch);
    RUN_TEST(test_unhappy_category_mismatch);
    RUN_TEST(test_unhappy_missing_tag);
    RUN_TEST(test_unhappy_unregistered_widget);
    RUN_TEST(test_type_based_whitelisting);
    return UNITY_END();
}
