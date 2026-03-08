#include <unity.h>
#include "../../lib/Interfaces/IScreen.h"
#include "../_common/stubs/MockPainter.h"
#include "../../lib/Logic/Workflows/BlockerScreen.h"
#include "../../lib/Logic/Workflows/DashboardScreen.h"
#include "../../lib/Logic/Workflows/DashboardScreen.cpp"
#include "../../lib/Logic/Workflows/ShotScreen.h"
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
    BlockerScreen screen(&blocker, &registry);
    
    MockPainter painter;
    screen.paint(painter); 
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(WidgetType::STATUS, painter.lastComposition.widgets[0].type);
}

void test_dashboardscreen_painting() {
    DashboardScreen screen(nullptr); 
    
    MockPainter painter;
    screen.paint(painter);
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(4, painter.lastComposition.widgets.size());
}

void test_shotscreen_painting() {
    ShotScreen screen(nullptr);
    MockPainter painter;
    screen.paint(painter);
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_EQUAL(2, painter.lastComposition.widgets.size());
}
