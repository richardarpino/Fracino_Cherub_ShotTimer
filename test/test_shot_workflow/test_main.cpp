#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/Workflows/WorkflowEngine.h"
#include "../../lib/Logic/Workflows/WorkflowEngine.cpp"
#include "../../lib/Logic/Workflows/BasicWorkflow.h"
#include "../../lib/Logic/Workflows/BasicWorkflow.cpp"
#include "../../lib/Logic/Workflows/ShotInProgressScreen.h"
#include "../../lib/Logic/Workflows/ShotInProgressScreen.cpp"
#include "../../lib/Logic/Workflows/ShotSummaryScreen.h"
#include "../../lib/Logic/Workflows/ShotSummaryScreen.cpp"
#include "../../lib/UI/ScreenLayout.cpp"
#include "../../lib/UI/SensorWidget.cpp"
#include "../../lib/UI/StatusWidget.cpp"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../_common/stubs/SensorStub.h"

// Mock for LVGL functions used in init/applyTheme
extern "C" {
    lv_obj_t* lv_grid_create(lv_obj_t* parent) { return (lv_obj_t*)1; }
    void lv_obj_set_size(lv_obj_t* obj, lv_coord_t w, lv_coord_t h) {}
    void lv_grid_set_column_dsc_array(lv_obj_t* obj, const lv_coord_t col_dsc[]) {}
    void lv_grid_set_row_dsc_array(lv_obj_t* obj, const lv_coord_t row_dsc[]) {}
    void lv_obj_set_grid_cell(lv_obj_t* obj, lv_grid_align_t x_align, int8_t col_pos, uint8_t col_span, lv_grid_align_t y_align, int8_t row_pos, uint8_t row_span) {}
    lv_obj_t* lv_obj_create(lv_obj_t* parent) { return (lv_obj_t*)1; }
    lv_obj_t* lv_label_create(lv_obj_t* parent) { return (lv_obj_t*)1; }
    void lv_label_set_text(lv_obj_t* obj, const char* text) {}
    void lv_obj_set_style_bg_color(lv_obj_t* obj, lv_color_t value, lv_style_selector_t selector) {}
    void lv_obj_set_style_text_color(lv_obj_t* obj, lv_color_t value, lv_style_selector_t selector) {}
    void lv_obj_set_style_border_width(lv_obj_t* obj, lv_coord_t value, lv_style_selector_t selector) {}
    void lv_obj_set_style_pad_all(lv_obj_t* obj, lv_coord_t value, lv_style_selector_t selector) {}
    void lv_obj_set_style_text_font(lv_obj_t* obj, const lv_font_t* value, lv_style_selector_t selector) {}
    void lv_obj_align(lv_obj_t* obj, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs) {}
}

void test_shot_workflow_transitions() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    
    SensorStub pumpStub;
    registry.provide<PumpReading>(&pumpStub);
    
    BasicWorkflow dashboard;
    // MockScreen would be better but let's test the real components in isolation if possible
    // For now, let's just use the engine logic
    
    BasicWorkflow shotWorkflow;
    ShotInProgressScreen inProgress(&registry);
    ShotSummaryScreen summary(&registry);
    
    shotWorkflow.addScreen(&inProgress);
    shotWorkflow.addScreen(&summary);
    
    engine.setDefaultWorkflow(&dashboard);
    engine.addTriggerWorkflow(&shotWorkflow, TriggerType::PUMP, 100);
    
    // 1. Initial State: Pump OFF -> Dashboard
    pumpStub.setReading(0.0f);
    registry.update();
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&dashboard, engine.getActiveWorkflow());
    
    // 2. Pump ON -> ShotWorkflow (InProgress)
    pumpStub.setReading(1.0f);
    registry.update();
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&shotWorkflow, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&inProgress, engine.getActiveScreen());
    
    // 3. Update while InProgress
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&inProgress, engine.getActiveScreen());
    
    // 4. Pump OFF -> Advance to Summary
    pumpStub.setReading(0.0f);
    registry.update();
    engine.update(); // This should trigger inProgress->isDone() -> next()
    TEST_ASSERT_EQUAL_PTR(&shotWorkflow, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&summary, engine.getActiveScreen());
}

void test_shot_summary_hold_logic() {
    SensorDispatcher registry;
    ShotSummaryScreen summary(&registry);
    
    // Initial state
    setHardwareTime(0);
    TEST_ASSERT_FALSE(summary.isDone());
    
    // Set start time
    summary.update();
    TEST_ASSERT_EQUAL(0, millis());
    
    // Advance time
    setHardwareTime(5000);
    TEST_ASSERT_FALSE(summary.isDone());
    
    setHardwareTime(10001);
    TEST_ASSERT_TRUE(summary.isDone());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_shot_workflow_transitions);
    RUN_TEST(test_shot_summary_hold_logic);
    return UNITY_END();
}
