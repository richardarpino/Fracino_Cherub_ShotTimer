#include <unity.h>
#include "../../lib/Interfaces/IScreen.h"
#include "../_common/stubs/MockPainter.h"
#include "../../lib/Logic/Workflows/GenericScreen.h"
#include "../../lib/Interfaces/SensorTags.h"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../_common/stubs/BlockerStub.h"

// Keep the old infrastructure test
class TestScreen : public IScreen {
public:
    void update() override {}
    bool isDone() const override { return true; }
    ScreenComposition getComposition() const override {
        return ScreenComposition(1, 1).add(WidgetType::STATUS, "TestTag");
    }
    void paint(IPainter& p) override {
        p.draw(getComposition(), nullptr);
    }
};

void test_painter_infrastructure() {
    TestScreen screen;
    MockPainter painter;
    screen.paint(painter);
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount); // draw() incremented this for compatibility
    TEST_ASSERT_EQUAL(1, painter.lastComposition.widgets.size());
}

void test_blockerscreen_painting() {
    BlockerStub blocker;
    SensorDispatcher registry;
    GenericScreen screen(ScreenComposition(1, 1).add(WidgetType::STATUS, "Blocker"), &registry);
    
    MockPainter painter;
    screen.paint(painter); 
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(WidgetType::STATUS, painter.lastComposition.widgets[0].type);
}

void test_dashboardscreen_painting() {
    GenericScreen screen(
        ScreenComposition(2, 2)
            .add(WidgetType::GAUGE, BoilerPressureReading::NAME)
            .add(WidgetType::SENSOR, BoilerTempReading::NAME)
            .add(WidgetType::SENSOR, SystemUptimeReading::NAME)
            .add(WidgetType::SENSOR, HeatingCycleReading::NAME),
        nullptr
    ); 
    
    MockPainter painter;
    screen.paint(painter);
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(4, painter.lastComposition.widgets.size());
}

void test_shotscreen_painting() {
    GenericScreen screen(
        ScreenComposition(2, 1)
            .add(WidgetType::SENSOR, LastValidShotReading::NAME)
            .add(WidgetType::SENSOR, ShotTimeReading::NAME),
        nullptr
    );
    MockPainter painter;
    screen.paint(painter);
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(2, painter.lastComposition.widgets.size());
}
