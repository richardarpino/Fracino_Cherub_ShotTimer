#include <unity.h>
#include "../../lib/UI/GaugeWidget.h"
#include "../../lib/UI/BlockerWidget.h"
#include "../../lib/Registry/ISensorRegistry.h"
#include "../../lib/Registry/WidgetTags.h"
#include "../../test/_common/stubs/Arduino.cpp"
#include "../../test_simulator/HeadlessDriver.cpp"
#include <iostream>

// Mock Registry for Phase 5 verification
class MockMetadataRegistry : public ISensorRegistry {
public:
    void update() override {}
    DataCategory getCategory(const char*) override { return DataCategory::TELEMETRY; }
    PhysicalQuantity getQuantity(const char*) override { return PhysicalQuantity::PRESSURE; }
    
    SensorMetadata getSensorMetadataByName(const char* name) override {
        if (std::string(name) == "BoilerPressure") {
            SensorMetadata meta;
            meta.low = Reading(0.0f, "BAR", "Pressure", 1);
            meta.high = Reading(3.0f, "BAR", "Pressure", 1);
            meta.init = Reading(0.0f, "BAR", "Pressure", 1);
            return meta;
        }
        return SensorMetadata();
    }

    ServiceMetadata getServiceMetadataByName(const char* name) override { 
        if (std::string(name) == "WiFi") {
            ServiceMetadata meta;
            meta.ready.title = "WIFI READY";
            meta.pending.title = "WIFI SETUP";
            return meta;
        }
        return ServiceMetadata(); 
    }
    
    void storeMetadataInternal(const char*, SensorMetadata) override {}
    void storeMetadataInternal(const char*, ServiceMetadata) override {}

    Reading getReadingByName(const char*) override { return Reading(1.5f, "BAR", "Pressure", 1); }
    void setReadingByName(const char*, Reading) override {}
    
    StatusMessage getStatusByName(const char* name) override { 
        if (std::string(name) == "WiFi") return StatusMessage("", "Connecting...", 50.0f);
        return StatusMessage(); 
    }
    void setStatusByName(const char*, StatusMessage) override {}
    void attachProcessorInternal(const char*, class ITagProcessor*) override {}
    bool hasProcessor(const char*) override { return false; }
    void triggerResolution(const char*) override {}
};

void test_gauge_metadata_configuration() {
    HeadlessDriver::init(240, 135);
    MockMetadataRegistry registry;
    
    // Create a Late-Binding Gauge for "BoilerPressure"
    GaugeWidget<void> gauge("BoilerPressure", &registry);
    
    gauge.init(lv_scr_act(), 2, 2);
    gauge.refresh();
    
    // VERIFICATION (RED Phase): 
    // We expect 0.0 to 3.0 BAR for Boiler Pressure.
    // Currently, it will fail because GaugeWidget doesn't pull metadata yet.
    TEST_ASSERT_EQUAL_FLOAT(0.0f, gauge.getMin());
    TEST_ASSERT_EQUAL_FLOAT(3.0f, gauge.getMax());
}

void test_blocker_metadata_configuration() {
    HeadlessDriver::init(240, 135);
    MockMetadataRegistry registry;
    
    BlockerWidget blocker("WiFi");
    blocker.setRegistry(&registry);
    blocker.init(lv_scr_act(), 1, 1);
    blocker.refresh();
    
    // Title should come from ServiceMetadata.ready.title ("WIFI READY")
    TEST_ASSERT_EQUAL_STRING("WIFI READY", blocker.getTitle());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_gauge_metadata_configuration);
    RUN_TEST(test_blocker_metadata_configuration);
    return UNITY_END();
}
