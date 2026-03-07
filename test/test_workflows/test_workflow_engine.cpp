#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/Workflows/WorkflowEngine.h"
#include "../../lib/Logic/Workflows/WorkflowEngine.cpp"
#include "../../lib/Logic/Workflows/BasicWorkflow.h"
#include "../../lib/Logic/Workflows/BasicWorkflow.cpp"
#include "../_common/stubs/SensorStub.h"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"

class MockScreen : public IScreen {
public:
    MockScreen(const char* name) : _name(name) {}
    ScreenLayout* getLayout() override { return nullptr; }
private:
    const char* _name;
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
    engine.addTriggerWorkflow(&workflowB, TriggerType::BUTTON_LEFT, 10); // Higher precedence

    // Before trigger
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&workflowA, engine.getActiveWorkflow());

    // After trigger
    triggerStub.setReading(1.0f); // Button Pressed
    registry.update(); // MUST update registry to propagate to RegistrySwitch
    engine.update();
    TEST_ASSERT_EQUAL_PTR(&workflowB, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_PTR(&screenB, engine.getActiveScreen());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_workflow_sequential_navigation);
    RUN_TEST(test_workflow_lifecycle);
    RUN_TEST(test_workflow_engine_switching);
    return UNITY_END();
}
