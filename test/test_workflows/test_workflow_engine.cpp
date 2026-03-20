#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/Workflows/WorkflowEngine.h"
#include "../../lib/Logic/Workflows/WorkflowEngine.cpp"
#include "../../lib/Logic/Workflows/BasicWorkflow.h"
#include "../../lib/Logic/Workflows/BasicWorkflow.cpp"
#include "../../lib/Logic/Triggers/WorkflowRunningTrigger.h"
#include "../../lib/Interfaces/IPainter.h"
#include "../_common/stubs/SensorStub.h"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"

#include "../_common/stubs/BlockerStub.h"
#include "../../lib/Interfaces/ITrigger.h"
#include "../../lib/Registry/WidgetTags.h"
#include "../../lib/Factories/WorkflowFactory.h"
#include "../../lib/Logic/Workflows/GenericScreen.h"
#include "../../lib/Services/OTABlocker.h"
#include "../../lib/Services/OTABlocker.cpp"
#include "../../lib/Logic/Triggers/OTADownloadingTrigger.h"

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
    MockScreen(const char* name) : _name(name), _isDone(true) {}
    void update() override {}
    bool isDone() const override { return _isDone; }
    void setDone(bool done) { _isDone = done; }

    const char* getName() const override { return _name; }
    const char* getDescription() const override { return "Mock Description"; }
    const char* getExitCondition() const override { return "Mock Exit"; }

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

    const char* getName() const override { return "Mock Blocker Screen"; }
    const char* getDescription() const override { return "Mock Blocker Description"; }
    const char* getExitCondition() const override { return "Blocker Finished"; }

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
    MockScreen* screen1 = new MockScreen("Screen 1");
    MockScreen* screen2 = new MockScreen("Screen 2");

    workflow.addScreen(screen1);
    workflow.addScreen(screen2);

    // 1. Initial State
    TEST_ASSERT_EQUAL_PTR(screen1, workflow.getActiveScreen());

    // 2. Advance
    workflow.next();
    TEST_ASSERT_EQUAL_PTR(screen2, workflow.getActiveScreen());

    // 3. Boundaries
    workflow.next();
    TEST_ASSERT_EQUAL_PTR(screen2, workflow.getActiveScreen());
}

void test_workflow_lifecycle() {
    BasicWorkflow workflow;
    MockScreen* screen1 = new MockScreen("Screen 1");
    workflow.addScreen(screen1);

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
    MockBlockerScreen* screen1 = new MockBlockerScreen(&blocker1);

    BlockerStub blocker2;
    blocker2.setActive(true);
    MockBlockerScreen* screen2 = new MockBlockerScreen(&blocker2);

    startupWorkflow.addScreen(screen1);
    startupWorkflow.addScreen(screen2);
    engine.setRootWorkflow(&startupWorkflow);

    // 1. Initially on screen 1
    engine.update();
    TEST_ASSERT_EQUAL_PTR(screen1, engine.getActiveScreen());

    // 2. Unblock 1
    blocker1.setActive(false);
    engine.update();
    
    // Should have advanced to screen 2
    TEST_ASSERT_EQUAL_PTR(screen2, engine.getActiveScreen());

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
    MockScreen* screenA = new MockScreen("Screen A");
    workflowA.addScreen(screenA);

    BasicWorkflow workflowB;
    MockScreen* screenB = new MockScreen("Screen B");
    workflowB.addScreen(screenB);

    // Initial Workflow
    engine.setRootWorkflow(&workflowA);
    TEST_ASSERT_EQUAL_PTR(&workflowA, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(screenA, engine.getActiveScreen());

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
    TEST_ASSERT_EQUAL_PTR(screenB, engine.getActiveScreen());
}

// No includes for real BlockerScreen to avoid UI dependencies in native

void test_workflow_engine_default_fallback() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);

    BasicWorkflow startup;
    BlockerStub blocker;
    blocker.setActive(true);
    MockBlockerScreen* startupScreen = new MockBlockerScreen(&blocker);
    startup.addScreen(startupScreen);

    BasicWorkflow dashboard;
    MockScreen* dashScreen = new MockScreen("Dash");
    dashboard.addScreen(dashScreen);

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
    
    BasicWorkflow* rootWf = new BasicWorkflow("P1", "d1");
    MockScreen* screen1 = new MockScreen("Screen 1");
    screen1->setDone(false); // Blocking
    rootWf->addScreen(screen1);
    engine.setRootWorkflow(rootWf);

    BasicWorkflow* triggeredWf = new BasicWorkflow("P2", "d2");
    MockScreen* screen2 = new MockScreen("Screen 2");
    screen2->setDone(false); // Blocking
    triggeredWf->addScreen(screen2);
    
    MockTrigger trigger;
    engine.addTriggerWorkflow(triggeredWf, &trigger, 100);

    setHardwareTime(1000);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(screen1, engine.getActiveScreen());

    // 1. Activate Trigger
    trigger.setActive(true);
    engine.update();
    
    // Should still see screen 1 visually because transition started
    TEST_ASSERT_EQUAL_PTR(triggeredWf, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(screen1, engine.getActiveScreen());

    // 2. Move time forward 500ms (Still in transition)
    addHardwareTime(500);
    engine.update();
    TEST_ASSERT_EQUAL_PTR_MESSAGE(screen1, engine.getActiveScreen(), 
        "Engine failed to hold visual transition screen!");

    // 3. Move time past 1000ms
    addHardwareTime(600);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(screen2, engine.getActiveScreen());

    // p1, p2 will be deleted by engine/nodes
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
    MockScreen* screen1 = new MockScreen("Screen 1");
    rootWf.addScreen(screen1);
    engine.setRootWorkflow(&rootWf);

    BasicWorkflow triggeredWf;
    FastMockScreen* screen2 = new FastMockScreen("Screen 2"); // Returns 0ms delay
    triggeredWf.addScreen(screen2);
    
    MockTrigger trigger;
    engine.addTriggerWorkflow(&triggeredWf, &trigger, 100);

    setHardwareTime(1000);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(screen1, engine.getActiveScreen());

    // 1. Activate Trigger - Should switch INSTANTLY
    trigger.setActive(true);
    engine.update();
    
    // Should see screen 2 immediately because it overrides to 0ms
    TEST_ASSERT_EQUAL_PTR(&triggeredWf, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(screen2, engine.getActiveScreen());
}

void test_painter_infrastructure(); // From test_painter.cpp
void test_blockerscreen_painting(); // From test_painter.cpp
void test_dashboardscreen_painting(); // From test_painter.cpp
void test_shotscreen_painting(); // From test_painter.cpp

void test_screen_metadata() {
    SensorDispatcher registry;
    ScreenComposition comp(1, 1);
    
    GenericScreen screen(
        comp, 
        &registry, 
        "Test Screen", 
        "A screen for testing metadata", 
        "Condition Met"
    );

    TEST_ASSERT_EQUAL_STRING("Test Screen", screen.getName());
    TEST_ASSERT_EQUAL_STRING("A screen for testing metadata", screen.getDescription());
    TEST_ASSERT_EQUAL_STRING("Condition Met", screen.getExitCondition());
}

void test_workflow_metadata() {
    BasicWorkflow workflow("Test Workflow", "A workflow for testing metadata");

    TEST_ASSERT_EQUAL_STRING("Test Workflow", workflow.getName());
    TEST_ASSERT_EQUAL_STRING("A workflow for testing metadata", workflow.getDescription());
}

void test_workflow_factory_assembly() {
    SensorDispatcher registry;
    BlockerStub wifi, ota, warmup;
    
    IWorkflow* systemWf = WorkflowFactory::createSystemWorkflow(&registry, &wifi, &ota, &warmup);
    
    TEST_ASSERT_NOT_NULL(systemWf);
    TEST_ASSERT_EQUAL_STRING("System Startup", systemWf->getName());
    
    // Check screens (WiFi, OTA, Warmup)
    // We can't easily iterate without adding more interface methods, but we can check if it finishes
    // For now, metadata is the priority
    
    delete systemWf;
}

#include "../../lib/Logic/Workflows/WorkflowNode.h"

void test_node_hierarchy() {
    BasicWorkflow rootWf("Root", "root");
    BasicWorkflow childWf("Child", "child");
    MockTrigger trigger;

    WorkflowNode rootNode(&rootWf);
    WorkflowNode* childNode = new WorkflowNode(&childWf);
    rootNode.addChild(childNode, &trigger, 100);

    TEST_ASSERT_EQUAL_PTR(&rootWf, rootNode.getWorkflow());
    TEST_ASSERT_EQUAL(1, rootNode.getChildren().size());
    TEST_ASSERT_EQUAL_PTR(&childWf, rootNode.getChildren()[0].node->getWorkflow());
}

void test_blocking_traversal() {
    MockScreen* blockerScreen = new MockScreen("Blocker");
    blockerScreen->setDone(false); // It's blocking!
    
    BasicWorkflow* rootWf = new BasicWorkflow("Root", "root");
    rootWf->addScreen(blockerScreen);

    BasicWorkflow* childWf = new BasicWorkflow("Child", "child");
    childWf->addScreen(new MockScreen("ChildScreen"));
    
    MockTrigger* trigger = new MockTrigger();
    trigger->setActive(true); // Trigger is active, but parent should block

    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    engine.setRootWorkflow(rootWf);
    engine.addTriggerWorkflow(childWf, trigger, 100);

    engine.update();
    
    // NEW EXPECTATION: Hierarchical pre-emption (Phase 8 logic)
    // Even if root is blocking, an active child trigger should pre-empt it.
    TEST_ASSERT_EQUAL_PTR_MESSAGE(childWf, engine.getActiveWorkflow(), 
        "Engine failed to pre-empt a blocking parent with an active child trigger!");
    TEST_ASSERT_EQUAL_STRING("ChildScreen", engine.getActiveScreen()->getName());
}

void test_shot_timer_return() {
    BasicWorkflow* dashWf = new BasicWorkflow("Dashboard", "dash");
    dashWf->addScreen(new MockScreen("DashScreen"));

    BasicWorkflow* shotWf = new BasicWorkflow("Shot", "shot");
    shotWf->addScreen(new MockScreen("ShotScreen"));
    
    MockTrigger* pumpTrigger = new MockTrigger();
    pumpTrigger->setActive(true);

    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    engine.setRootWorkflow(dashWf);
    engine.addTriggerWorkflow(shotWf, pumpTrigger, 100);

    engine.update();
    TEST_ASSERT_EQUAL_PTR(shotWf, engine.getActiveWorkflow());

    // DEACTIVATE Trigger: Should return to Dashboard
    pumpTrigger->setActive(false);
    engine.update();
    
    TEST_ASSERT_EQUAL_PTR(dashWf, engine.getActiveWorkflow());
}

void test_deep_hierarchy_traversal() {
    BasicWorkflow* rootWf = new BasicWorkflow("Root", "root");
    rootWf->addScreen(new MockScreen("RootScreen"));

    BasicWorkflow* childWf = new BasicWorkflow("Child", "child");
    childWf->addScreen(new MockScreen("ChildScreen"));

    BasicWorkflow* grandChildWf = new BasicWorkflow("GrandChild", "gc");
    grandChildWf->addScreen(new MockScreen("GrandChildScreen"));

    MockTrigger* childTrigger = new MockTrigger();
    childTrigger->setActive(true);

    MockTrigger* grandChildTrigger = new MockTrigger();
    grandChildTrigger->setActive(true);

    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    engine.setRootWorkflow(rootWf);
    
    // Root -> Child
    engine.addTriggerWorkflow(childWf, childTrigger, 100, rootWf);
    // Child -> GrandChild
    engine.addTriggerWorkflow(grandChildWf, grandChildTrigger, 100, childWf);

    engine.update();
    
    // Should traverse all the way to GrandChild
    TEST_ASSERT_EQUAL_PTR(grandChildWf, engine.getActiveWorkflow());

    // Deactivate GrandChild trigger -> should return to Child
    grandChildTrigger->setActive(false);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(childWf, engine.getActiveWorkflow());

    // Deactivate Child trigger -> should return to Root
    childTrigger->setActive(false);
    engine.update();
    TEST_ASSERT_EQUAL_PTR(rootWf, engine.getActiveWorkflow());
}

void test_global_intercept() {
    MockScreen* blockerScreen = new MockScreen("Blocker");
    blockerScreen->setDone(false); // Blocking
    
    BasicWorkflow* rootWf = new BasicWorkflow("Root", "root");
    rootWf->addScreen(blockerScreen);

    BasicWorkflow* alarmWf = new BasicWorkflow("Alarm", "alarm");
    alarmWf->addScreen(new MockScreen("AlarmScreen"));
    
    MockTrigger* alarmTrigger = new MockTrigger();
    alarmTrigger->setActive(true);

    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    engine.setRootWorkflow(rootWf);
    
    // Add as Global Trigger (High precedence)
    engine.addGlobalTrigger(alarmWf, alarmTrigger, 999);

    engine.update();
    
    // Should override even the blocker because it's global
    TEST_ASSERT_EQUAL_PTR(alarmWf, engine.getActiveWorkflow());
}

void test_breadcrumb_generation() {
    BasicWorkflow* rootWf = new BasicWorkflow("System", "sys");
    BasicWorkflow* childWf = new BasicWorkflow("Dash", "dash");
    BasicWorkflow* grandChildWf = new BasicWorkflow("Shot", "shot");

    MockTrigger* t1 = new MockTrigger(); t1->setActive(true);
    MockTrigger* t2 = new MockTrigger(); t2->setActive(true);

    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    engine.setRootWorkflow(rootWf);
    engine.addTriggerWorkflow(childWf, t1, 1, rootWf);
    engine.addTriggerWorkflow(grandChildWf, t2, 1, childWf);

    engine.update();
    
    // Initially should be System > Dash > Shot
    // Note: Use a fixed buffer or std::string if available, but here we expect a specific format.
    const char* bc = engine.getActiveBreadcrumb();
    TEST_ASSERT_EQUAL_STRING("System > Dash > Shot", bc);

    // Deactivate grandchild -> System > Dash
    t2->setActive(false);
    engine.update();
    TEST_ASSERT_EQUAL_STRING("System > Dash", engine.getActiveBreadcrumb());
}

#include "../../lib/Logic/Triggers/OTADownloadingTrigger.h"

void test_ota_global_interrupt() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    
    BasicWorkflow* mainWf = new BasicWorkflow("Main", "main");
    BasicWorkflow* otaWf = new BasicWorkflow("OTA", "ota");
    
    engine.setRootWorkflow(mainWf);
    
    // Phase 1 (RED): Uses real class but it currently returns false
    OTADownloadingTrigger* otaTrigger = new OTADownloadingTrigger(&registry);
    
    engine.addGlobalTrigger(otaWf, otaTrigger, 100);
    
    // Initial state: Off
    registry.publish<OTAStatus>(StatusMessage("OTA", "OFF", 100.0f, false));
    engine.update();
    TEST_ASSERT_EQUAL_PTR(mainWf, engine.getActiveWorkflow());
    
    // Simulate OTA starting (Downloading at 50%)
    registry.publish<OTAStatus>(StatusMessage("OTA", "UPDATING", 50.0f, false));
    otaTrigger->update(); // Trigger needs update to see registry changes
    engine.update();
    
    // This SHOULD switch to otaWf, but in RED phase it will FAIL
    TEST_ASSERT_EQUAL_PTR(otaWf, engine.getActiveWorkflow());
}

void test_ota_screen_skip_bug() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry);
    
    // Simulate current state: Listening (100% progress)
    registry.publish<OTAStatus>(StatusMessage("OTA Update", "LISTENING...", 100.0f, false));
    
    OTABlocker* otaBlocker = new OTABlocker(&registry);
    BasicWorkflow* otaWf = new BasicWorkflow("OTA", "ota");
    otaWf->addScreen(new GenericScreen(ScreenComposition(), &registry, "OTA Screen", "desc", "exit", otaBlocker));
    
    OTADownloadingTrigger* otaTrigger = new OTADownloadingTrigger(&registry);
    engine.addGlobalTrigger(otaWf, otaTrigger, 1000);
    
    // Engine update while idle (Trigger false)
    engine.update();
    
    // Start OTA: Progress goes to 0% in the registry
    registry.publish<OTAStatus>(StatusMessage("OTA Update", "UPDATING...", 0.0f, false));
    
    // We DO NOT call otaBlocker->update() manually here.
    // We expect engine.update() to handle it.
    engine.update();
    
    // Check if we are on the OTA workflow
    TEST_ASSERT_EQUAL_PTR(otaWf, engine.getActiveWorkflow());
    
    // Check if the screen is active. 
    // In the BUGGY state, this will be NULL because it was skipped at the moment of switch.
    TEST_ASSERT_NOT_NULL(engine.getActiveScreen());
}

void test_workflow_hierarchy_skip_bug() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry, 0); // No transition pause for testing

    // Parent Workflow: 3 screens
    BasicWorkflow* parent = new BasicWorkflow("Parent", "Multi-screen");
    BlockerStub* b1 = new BlockerStub();
    BlockerStub* b2 = new BlockerStub();
    BlockerStub* b3 = new BlockerStub();
    parent->addScreen(new GenericScreen(ScreenComposition(), &registry, "S1", "Desc", "Exit", b1));
    parent->addScreen(new GenericScreen(ScreenComposition(), &registry, "S2", "Desc", "Exit", b2));
    parent->addScreen(new GenericScreen(ScreenComposition(), &registry, "S3", "Desc", "Exit", b3));

    // Child Workflow: 1 screen
    BasicWorkflow* child = new BasicWorkflow("Child", "Successor");
    BlockerStub* b4 = new BlockerStub();
    child->addScreen(new GenericScreen(ScreenComposition(), &registry, "S4", "Desc", "Exit", b4));

    class AlwaysTrigger : public ITrigger {
    public:
        void update() override {}
        bool isActive() const override { return true; }
    };
    static AlwaysTrigger always;

    engine.setRootWorkflow(parent);
    engine.setDefaultWorkflow(child);

    // Initial State: Parent Active, S1
    engine.update();
    TEST_ASSERT_EQUAL_STRING("Parent", engine.getActiveWorkflow()->getName());
    
    // Step 1: Finish S1.
    b1->setActive(true);
    engine.update(); // Land on S2

    TEST_ASSERT_EQUAL_STRING("Parent", engine.getActiveWorkflow()->getName());
    TEST_ASSERT_EQUAL_STRING("S2", engine.getActiveScreen()->getName());

    // Step 2: Finish S2.
    b2->setActive(true);
    engine.update(); // Land on S3

    TEST_ASSERT_EQUAL_STRING("Parent", engine.getActiveWorkflow()->getName());
    TEST_ASSERT_EQUAL_STRING("S3", engine.getActiveScreen()->getName());

    // Step 3: Finish S3.
    b3->setActive(true);
    engine.update(); //land on Parent (isDone but not processed yet)
    engine.update(); //land on Child (Default)

    TEST_ASSERT_EQUAL_STRING("Child", engine.getActiveWorkflow()->getName());
    TEST_ASSERT_EQUAL_STRING("S4", engine.getActiveScreen()->getName());

    // Clean up
    delete b1; delete b2; delete b3; delete b4;
}

class AlwaysTrigger : public ITrigger {
public:
    void update() override {}
    bool isActive() const override { return true; }
};

void test_sibling_succession_pattern() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry, 0);

    // Root (System) - Container
    BasicWorkflow* rootWf = new BasicWorkflow("System", "root");

    // Peer 1 (Startup) - Prio 10
    BasicWorkflow* p1 = new BasicWorkflow("Startup", "p1");
    BlockerStub* b1 = new BlockerStub();
    p1->addScreen(new GenericScreen(ScreenComposition(), &registry, "S1", "d", "e", b1));

    // Peer 2 (Dashboard) - Prio 1
    BasicWorkflow* p2 = new BasicWorkflow("Dashboard", "p2");
    BlockerStub* b2 = new BlockerStub(); // Persistent block (isActive = false)
    p2->addScreen(new GenericScreen(ScreenComposition(), &registry, "S2", "d", "e", b2));

    // Grandchild (Shot) - child of Dashboard
    BasicWorkflow* gc = new BasicWorkflow("Shot", "gc");
    BlockerStub* b3 = new BlockerStub();
    gc->addScreen(new GenericScreen(ScreenComposition(), &registry, "S3", "d", "e", b3));

    AlwaysTrigger always;
    MockTrigger shotTrigger;
    
    WorkflowRunningTrigger p1Running(p1);
    
    engine.setRootWorkflow(rootWf);
    engine.addTriggerWorkflow(p1, &p1Running, 10, rootWf); // Startup wins initially
    engine.addTriggerWorkflow(p2, &always, 1, rootWf);  // Dashboard is fallback sibling
    engine.addTriggerWorkflow(gc, &shotTrigger, 100, p2); // Shot is child of Dashboard

    // 1. Initial State -> Startup wins
    engine.update();
    TEST_ASSERT_EQUAL_STRING("Startup", engine.getActiveWorkflow()->getName());

    // 2. Startup finishes
    b1->setActive(true);
    engine.update(); // Land on Dashboard screen S2
    
    TEST_ASSERT_EQUAL_STRING("Dashboard", engine.getActiveWorkflow()->getName());
    TEST_ASSERT_EQUAL_STRING("S2", engine.getActiveScreen()->getName());

    // 3. Activate Shot trigger
    shotTrigger.setActive(true);
    engine.update();

    // SUCCESS CHECK: Did we reach Shot?
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Shot", engine.getActiveWorkflow()->getName(), 
        "Engine failed to trigger grandchild from a sibling fallback workflow!");
    TEST_ASSERT_EQUAL_STRING("S3", engine.getActiveScreen()->getName());

    // Clean up
    delete b1; delete b2; delete b3;
}

void test_logic_continues_during_transition() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry, 1000); // 1s pause

    class MockWorkflow : public BasicWorkflow {
    public:
        int updateCount = 0;
        MockWorkflow(const char* name) : BasicWorkflow(name, "m") {}
        void update() override {
            updateCount++;
            BasicWorkflow::update();
        }
    };

    MockWorkflow* p1 = new MockWorkflow("P1");
    p1->addScreen(new GenericScreen(ScreenComposition(), &registry, "S1", "d", "e", nullptr));
    
    MockWorkflow* p2 = new MockWorkflow("P2");
    p2->addScreen(new GenericScreen(ScreenComposition(), &registry, "S2", "d", "e", nullptr));

    AlwaysTrigger always;
    MockTrigger t2;

    engine.setRootWorkflow(p1);
    engine.addTriggerWorkflow(p2, &t2, 100, p1);

    // 1. Initial
    engine.update(); // P1 active
    int firstP1Updates = p1->updateCount;
    TEST_ASSERT_TRUE(firstP1Updates > 0);

    // 2. Start transition to P2
    t2.setActive(true);
    engine.update(); // Detects T2, switches pointer, starts transition
    
    TEST_ASSERT_EQUAL_PTR(p2, engine.getActiveWorkflow());
    TEST_ASSERT_TRUE(p2->updateCount > 0); // First update on transition start

    // 3. Middle of transition (Wait 100ms)
    // BUG CHECK: Does p2 continue to update?
    engine.update();
    
    // EXPECTED: p2->updateCount should increase
    // ACTUAL: 1 (if engine returns early)
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, p2->updateCount, 
        "Engine logic blocked during transition pause!");
}

void test_nested_preemption_failure() {
    SensorDispatcher registry;
    WorkflowEngine engine(&registry, 0); // Immediate transition

    // 1. Root (System)
    BasicWorkflow* rootWf = new BasicWorkflow("Root", "r");
    
    // 2. Parent (Dashboard) - child of Root
    BasicWorkflow* p = new BasicWorkflow("Dashboard", "p");
    BlockerStub* b = new BlockerStub(); // Persistent block
    p->addScreen(new GenericScreen(ScreenComposition(), &registry, "Display", "d", "e", b));

    // 3. Child (Shot) - child of Parent
    BasicWorkflow* c = new BasicWorkflow("Shot", "c");
    c->addScreen(new GenericScreen(ScreenComposition(), &registry, "Timer", "d", "e", nullptr));

    AlwaysTrigger always;
    MockTrigger t;

    engine.setRootWorkflow(rootWf);
    engine.addTriggerWorkflow(p, &always, 1, rootWf); 
    engine.addTriggerWorkflow(c, &t, 100, p); // Shot nested under Dashboard

    // Initial state -> Dashboard active
    engine.update();
    TEST_ASSERT_EQUAL_STRING("Dashboard", engine.getActiveWorkflow()->getName());

    // Activate Shot trigger
    t.setActive(true);
    engine.update();

    // EXPECTED: "Shot" should pre-empt "Dashboard".
    // ACTUAL: It will stay on "Dashboard" because Dashboard is !isDone().
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Shot", engine.getActiveWorkflow()->getName(), 
        "Engine failed to descend into children because parent is blocking!");
}

void test_registry_status_delivery_integrity() {
    SensorDispatcher registry;
    
    // 1. Tag name
    const char* tag = WarmingUpStatus::NAME;
    
    // 2. Publish initial state (what WUP does in constructor)
    StatusMessage init("Warming Up...", "Starting", 0.0f, false);
    registry.publish<WarmingUpStatus>(init);
    
    // 3. Retrieve
    StatusMessage result = registry.getLatest<WarmingUpStatus>();
    
    // BUG CHECK: Did we get our 0% progress or did it fall back to metadata's 100%?
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f, result.progress, 
        "Registry failed to return published status progress (likely metadata shadowing)");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Warming Up...", result.title, 
        "Registry failed to return published status title");
}

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
    RUN_TEST(test_screen_metadata);
    RUN_TEST(test_workflow_metadata);
    RUN_TEST(test_workflow_factory_assembly);
    RUN_TEST(test_node_hierarchy);
    RUN_TEST(test_blocking_traversal);
    RUN_TEST(test_shot_timer_return);
    RUN_TEST(test_deep_hierarchy_traversal);
    RUN_TEST(test_global_intercept);
    RUN_TEST(test_breadcrumb_generation);
    RUN_TEST(test_ota_global_interrupt);
    RUN_TEST(test_ota_screen_skip_bug);
    RUN_TEST(test_workflow_hierarchy_skip_bug);
    RUN_TEST(test_registry_status_delivery_integrity);
    RUN_TEST(test_sibling_succession_pattern);
    RUN_TEST(test_logic_continues_during_transition);
    RUN_TEST(test_nested_preemption_failure);
    return UNITY_END();
}
