#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/Workflows/WorkflowEngine.h"
#include "../../lib/Logic/Workflows/WorkflowEngine.cpp"
#include "../../lib/Logic/Workflows/BasicWorkflow.h"
#include "../../lib/Logic/Workflows/BasicWorkflow.cpp"
#include "../../lib/Interfaces/IPainter.h"
#include "../_common/stubs/SensorStub.h"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"

#include "../_common/stubs/BlockerStub.h"
#include "../../lib/Interfaces/ITrigger.h"
#include "../../lib/Registry/WidgetTags.h"

class MockTrigger : public ITrigger {
public:
    MockTrigger() : _active(false), _updated(false) {}
    void update() override { _updated = true; }
    bool isActive() const override { return _active; }
    void setActive(bool active) { _active = active; }
    bool wasUpdated() const { return _updated; }
private:
    bool _active;
    bool _updated;
};

class MockScreen : public IScreen {
public:
    MockScreen(const char* name) : _name(name), _isDone(false) {}
    void update() override {}
    bool isDone() const override { return _isDone; }
    void setDone(bool done) { _isDone = done; }

    ScreenComposition getComposition() const override {
        return ScreenComposition(1, 1).add(SensorWidgetTag::NAME, _name);
    }
    void paint(IPainter& p) override {
        p.draw(getComposition(), nullptr);
    }
private:
    const char* _name;
    bool _isDone;
};

class MockBlockerScreen : public IScreen {
public:
    MockBlockerScreen(IBlocker* blocker) : _blocker(blocker) {}
    void update() override {}
    bool isDone() const override { return !_blocker->isActive(); }

    ScreenComposition getComposition() const override {
        return ScreenComposition(1, 1).add(BlockerWidgetTag::NAME, "blocker");
    }
    void paint(IPainter& p) override {
        p.draw(getComposition(), nullptr);
    }
private:
    IBlocker* _blocker;
};

void test_workflow_sequential_navigation() {
    BasicWorkflow workflow;
    MockScreen screen1("Screen 1");
    MockScreen screen2("Screen 2");

    workflow.addScreen(&screen1);
    workflow.addScreen(&screen2);

    // 1. Initial State
    TEST_ASSERT_EQUAL_PTR(&screen1, workflow.getActiveScreen());

    // 2. Advance
    workflow.next();
    TEST_ASSERT_EQUAL_PTR(&screen2, workflow.getActiveScreen());

    // 3. Boundaries
    workflow.next();
    TEST_ASSERT_EQUAL_PTR(&screen2, workflow.getActiveScreen());
}

void test_workflow_lifecycle() {
    BasicWorkflow workflow;
    MockScreen screen1("Screen 1");
    workflow.addScreen(&screen1);

    TEST_ASSERT_FALSE(workflow.isFinished());
    
    workflow.next();
    TEST_ASSERT_TRUE(workflow.isFinished());
}

void test_workflow_auto_advance_on_blocker_done() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    BasicWorkflow startupWorkflow;

    BlockerStub blocker1;
    blocker1.setActive(true);
    MockBlockerScreen screen1(&blocker1);

    BlockerStub blocker2;
    blocker2.setActive(true);
    MockBlockerScreen screen2(&blocker2);

    startupWorkflow.addScreen(&screen1);
    startupWorkflow.addScreen(&screen2);
    engine.setRootWorkflow(&startupWorkflow);

    // 1. Initially on screen 1
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&screen1, engine.getActiveScreen());

    // 2. Unblock 1
    blocker1.setActive(false);
    engine.update();
    
    // Should have advanced to screen 2
    TEST_ASSERT_EQUAL_PTR(&screen2, engine.getActiveScreen());

    // 3. Unblock 2
    blocker2.setActive(false);
    engine.update();

    // Workflow should be finished
    TEST_ASSERT_TRUE(startupWorkflow.isFinished());
}

void test_workflow_engine_switching() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);

    BasicWorkflow workflowA;
    MockScreen screenA("A");
    workflowA.addScreen(&screenA);

    BasicWorkflow workflowB;
    MockScreen screenB("B");
    workflowB.addScreen(&screenB);

    // Initial Workflow
    engine.setRootWorkflow(&workflowA);
    TEST_ASSERT_EQUAL_PTR(&workflowA, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&screenA, engine.getActiveScreen());

    // Switch via Trigger (Precedence)
    SensorStub triggerStub;
    registry.provide<ButtonLeftReading>(&triggerStub);

    // Register Workflow B with Trigger
    MockTrigger mockTrigger;
    engine.addTriggerWorkflow(&workflowB, &mockTrigger, 10); // Higher precedence

    // Before trigger
    engine.update();
    TEST_ASSERT_TRUE(mockTrigger.wasUpdated());
    TEST_ASSERT_EQUAL_PTR(&workflowA, engine.getActiveWorkflow());

    // After trigger
    mockTrigger.setActive(true);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&workflowB, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&screenB, engine.getActiveScreen());
}

// No includes for real BlockerScreen to avoid UI dependencies in native

void test_workflow_engine_default_fallback() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);

    BasicWorkflow startup;
    BlockerStub blocker;
    blocker.setActive(true);
    MockBlockerScreen startupScreen(&blocker);
    startup.addScreen(&startupScreen);

    BasicWorkflow dashboard;
    MockScreen dashScreen("Dash");
    dashboard.addScreen(&dashScreen);

    engine.setRootWorkflow(&startup);
    engine.setDefaultWorkflow(&dashboard);

    // 1. Initially on Startup
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&startup, engine.getActiveWorkflow());

    // 2. Startup Finishes
    blocker.setActive(false);
    engine.update();
    
    // Should have moved to default dashboard
    TEST_ASSERT_EQUAL_PTR(&dashboard, engine.getActiveWorkflow());
}

void test_workflow_engine_transition_pause() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry, 1000); // 1s pause
    
    BasicWorkflow rootWf;
    MockScreen screen1("Screen 1");
    rootWf.addScreen(&screen1);
    engine.setRootWorkflow(&rootWf);

    BasicWorkflow triggeredWf;
    MockScreen screen2("Screen 2");
    triggeredWf.addScreen(&screen2);
    
    MockTrigger trigger;
    engine.addTriggerWorkflow(&triggeredWf, &trigger, 100);

    setHardwareTime(1000);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&screen1, engine.getActiveScreen());

    // 1. Activate Trigger
    trigger.setActive(true);
    engine.update();
    
    // Should still see screen 1 visually because transition started
    TEST_ASSERT_EQUAL_PTR(&triggeredWf, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&screen1, engine.getActiveScreen());

    // 2. Move time forward 500ms (Still in transition)
    addHardwareTime(500);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&screen1, engine.getActiveScreen());

    // 3. Move time past 1000ms
    addHardwareTime(600);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&screen2, engine.getActiveScreen());
}

class FastMockScreen : public MockScreen {
public:
    FastMockScreen(const char* name) : MockScreen(name) {}
    int getTransitionDelay() const override { return 0; }
};

void test_workflow_engine_transition_override() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry, 1000); // 1s global pause
    
    BasicWorkflow rootWf;
    MockScreen screen1("Screen 1");
    rootWf.addScreen(&screen1);
    engine.setRootWorkflow(&rootWf);

    BasicWorkflow triggeredWf;
    FastMockScreen screen2("Screen 2"); // Returns 0ms delay
    triggeredWf.addScreen(&screen2);
    
    MockTrigger trigger;
    engine.addTriggerWorkflow(&triggeredWf, &trigger, 100);

    setHardwareTime(1000);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&screen1, engine.getActiveScreen());

    // 1. Activate Trigger - Should switch INSTANTLY
    trigger.setActive(true);
    engine.update();
    
    // Should see screen 2 immediately because it overrides to 0ms
    TEST_ASSERT_EQUAL_PTR(&triggeredWf, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&screen2, engine.getActiveScreen());
}

void test_painter_infrastructure(); // From test_painter.cpp
void test_blockerscreen_painting(); // From test_painter.cpp
void test_dashboardscreen_painting(); // From test_painter.cpp
void test_shotscreen_painting(); // From test_painter.cpp

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_painter_infrastructure);
    RUN_TEST(test_blockerscreen_painting);
    RUN_TEST(test_shotscreen_painting);
    RUN_TEST(test_dashboardscreen_painting);
    RUN_TEST(test_workflow_sequential_navigation);
    RUN_TEST(test_workflow_lifecycle);
    RUN_TEST(test_workflow_auto_advance_on_blocker_done);
    RUN_TEST(test_workflow_engine_switching);
    RUN_TEST(test_workflow_engine_default_fallback);
    RUN_TEST(test_workflow_engine_transition_pause);
    RUN_TEST(test_workflow_engine_transition_override);
    return UNITY_END();
}
