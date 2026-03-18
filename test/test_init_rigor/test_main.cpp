#include <unity.h>
#include "../../lib/Factories/LVGLWidgetFactory.h"
#include "../../lib/UI/LVGLPainter.h"
#include "../../lib/Registry/ISensorRegistry.h"
#include "../../lib/Registry/WidgetTags.h"
#include "../../test/_common/stubs/Arduino.cpp"
#include <iostream>

// Mock registry for the test
class MockRegistry : public ISensorRegistry {
public:
    void update() override {}
    DataCategory getCategory(const char* name) override { return DataCategory::TELEMETRY; }
    PhysicalQuantity getQuantity(const char* name) override { return PhysicalQuantity::NONE; }
    void triggerResolution(const char* name) override {}
    bool hasProcessor(const char* name) override { return false; }
    
    Reading getReadingByName(const char* name) override { return Reading(); }
    void setReadingByName(const char* name, Reading reading) override {}
    StatusMessage getStatusByName(const char* name) override { return StatusMessage(); }
    void setStatusByName(const char* name, StatusMessage status) override {}
    void attachProcessorInternal(const char* targetTagName, class ITagProcessor* processor) override {}
};

// A widget that claims to be a status widget but is NOT a StatusWidgetBase
class MaliciousWidget : public IWidget {
public:
    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override { return nullptr; }
    void refresh() override {}
    void applyTheme(ITheme* theme) override {}
    bool isStatusWidget() override { return true; }
    // Does NOT inherit from StatusWidgetBase, so static_cast in ScreenLayout will be invalid
};

void test_layout_init_with_null_parent_crashes() {
    ScreenLayout layout;
    std::cout << "Attempting layout init with null parent (expecting crash)..." << std::endl;
    // On some systems lv_grid_create(NULL) returns NULL,
    // and lv_obj_set_grid_dsc_array(NULL) CRASHES.
    layout.init(nullptr);
}

void test_unsafe_static_cast_robustness() {
    // This is the one I really want to be green eventually
    ScreenLayout layout;
    MaliciousWidget* m = new MaliciousWidget();
    layout.addWidget(m);
    
    std::cout << "Attempting unsafe static_cast..." << std::endl;
    layout.showMessage("Trigger Crash");
}

#include "../test_simulator/HeadlessDriver.cpp"

void test_dangling_grid_crashes() {
    std::cout << "Step 0: Initializing HeadlessDriver..." << std::endl;
    HeadlessDriver::init(240, 135);

    lv_obj_t* mock_parent = lv_scr_act();
    if (!mock_parent) {
        std::cout << "Step 0: FAILED - Screen not created" << std::endl;
        return;
    }
    
    // Ensure screen is clean
    lv_obj_clean(mock_parent);

    ScreenLayout layout;
    MockRegistry registry;
    
    // Add a widget so update() has work to do
    SensorWidget<void>* w = new SensorWidget<void>("test_tag", &registry);
    layout.addWidget(w);
    
    std::cout << "Step 1: Init layout..." << std::endl;
    layout.init(mock_parent);
    
    std::cout << "Step 2: Simulate ShotDisplay cleanup (deleting grid via clean)..." << std::endl;
    lv_obj_clean(mock_parent); 
    
    std::cout << "Step 3: Re-init with same parent (should handle dangling pointer)..." << std::endl;
    layout.init(mock_parent);
    
    std::cout << "Step 4: Update (should NOT crash)..." << std::endl;
    layout.update();
    std::cout << "Step 5: Test Finished Successfully" << std::endl;
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_layout_init_with_null_parent_crashes);
    RUN_TEST(test_unsafe_static_cast_robustness);
    RUN_TEST(test_dangling_grid_crashes);
    return UNITY_END();
}
