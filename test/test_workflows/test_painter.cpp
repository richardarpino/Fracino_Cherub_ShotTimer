#include <unity.h>
#include "../../lib/Interfaces/IScreen.h"
#include "../_common/stubs/MockPainter.h"
#include "../../lib/Logic/Workflows/BlockerScreen.h"
#include "../../lib/Logic/Workflows/DashboardScreen.h"
#include "../../lib/Logic/Workflows/DashboardScreen.cpp"
#include "../_common/stubs/BlockerStub.h"

// Keep the old infrastructure test
class TestScreen : public IScreen {
public:
    void update() override {}
    bool isDone() const override { return true; }
    void paint(IPainter& p) override {
        p.drawBlocker("Test Title", "Test", 100.0f, false);
    }
};

void test_painter_infrastructure() {
    TestScreen screen;
    MockPainter painter;
    screen.paint(painter);
    TEST_ASSERT_EQUAL(1, painter.drawBlockerCallCount);
}

void test_blockerscreen_painting() {
    BlockerStub blocker;
    // We need a way to mock StatusMessage since BlockerScreen uses it.
    // Wait, BlockerStub returns StatusMessage? Let's check BlockerStub below if needed.
    // For now, let's just create the screen
    BlockerScreen screen(&blocker);
    
    MockPainter painter;
    screen.paint(painter); // Should call drawBlocker
    
    TEST_ASSERT_EQUAL(1, painter.drawBlockerCallCount);
}

void test_dashboardscreen_painting() {
    // Pure logic test - DashboardScreen knows NOTHING about UI widgets or LVGL
    DashboardScreen screen(nullptr); // nullptr registry is fine for this test
    
    MockPainter painter;
    screen.paint(painter);
    
    TEST_ASSERT_EQUAL(1, painter.drawDashboardCallCount);
    TEST_ASSERT_NULL(painter.lastRegistryPassed);
}
