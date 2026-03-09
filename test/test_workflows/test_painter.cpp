#include <unity.h>
#include "../../lib/Interfaces/IScreen.h"
#include "../_common/stubs/MockPainter.h"
#include "../../lib/Logic/Workflows/GenericScreen.h"
#include "../../lib/Interfaces/SensorTags.h"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../_common/stubs/BlockerStub.h"
#include "../../lib/Registry/WidgetTags.h"

// Keep the old infrastructure test
class TestScreen : public IScreen {
public:
    void update() override {}
    bool isDone() const override { return true; }
    ScreenComposition getComposition() const override {
        return ScreenComposition(1, 1).add(BlockerWidgetTag::NAME, "TestTag");
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
    GenericScreen screen(ScreenComposition(1, 1).add(BlockerWidgetTag::NAME, "Blocker"), &registry);
    
    MockPainter painter;
    screen.paint(painter); 
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL_STRING(BlockerWidgetTag::NAME, painter.lastComposition.widgets[0].widgetName);
}

void test_dashboardscreen_painting() {
    GenericScreen screen(
        ScreenComposition(2, 2)
            .add(GaugeWidgetTag::NAME, BoilerPressureReading::NAME)
            .add(SensorWidgetTag::NAME, BoilerTempReading::NAME)
            .add(SensorWidgetTag::NAME, SystemUptimeReading::NAME)
            .add(SensorWidgetTag::NAME, HeatingCycleReading::NAME),
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
            .add(SensorWidgetTag::NAME, LastValidShotReading::NAME)
            .add(SensorWidgetTag::NAME, ShotTimeReading::NAME),
        nullptr
    );
    MockPainter painter;
    screen.paint(painter);
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(2, painter.lastComposition.widgets.size());
}
