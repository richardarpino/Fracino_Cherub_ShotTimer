#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Logic/Workflows/WorkflowEngine.h"
#include "../../lib/Logic/Workflows/WorkflowEngine.cpp"
#include "../../lib/Logic/Workflows/BasicWorkflow.h"
#include "../../lib/Logic/Workflows/BasicWorkflow.cpp"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/Workflows/GenericScreen.h"
#include "../_common/stubs/SensorStub.h"
#include "../_common/stubs/BlockerStub.h"

// Simple trigger that is always active
class AlwaysTrigger : public ITrigger {
public:
    void update() override {}
    bool isActive() const override { return true; }
};

void test_workflow_hierarchy_skip_bug() {
    MockRegistry registry;
    WorkflowEngine engine(&registry, 0); // No transition pause for testing

    // Parent Workflow: 2 screens
    BasicWorkflow* parent = new BasicWorkflow("Parent", "Multi-screen");
    MockBlocker* b1 = new MockBlocker();
    MockBlocker* b2 = new MockBlocker();
    parent->addScreen(new GenericScreen(ScreenComposition(), b1));
    parent->addScreen(new GenericScreen(ScreenComposition(), b2));

    // Child Workflow: 1 screen
    BasicWorkflow* child = new BasicWorkflow("Child", "Successor");
    MockBlocker* b3 = new MockBlocker();
    child->addScreen(new GenericScreen(ScreenComposition(), b3));

    AlwaysTrigger always;
    engine.setRootWorkflow(parent);
    engine.addTriggerWorkflow(child, &always, 1, parent);

    // Initial State: Parent Active, Screen 1
    engine.update();
    TEST_ASSERT_EQUAL_PTR(parent, engine.getActiveWorkflow());
    TEST_ASSERT_EQUAL_STRING("Parent", engine.getActiveWorkflow()->getName());
    
    // Step 1: Finish Screen 1
    b1->setActive(true);
    engine.update();

    // BUG CHECK: Should we be on Parent (Screen 2) or have we skipped to Child?
    // EXPECTED: Parent (Screen 2)
    // ACTUAL (Buggy): Child
    TEST_ASSERT_EQUAL_STRING_MESSAGE("Parent", engine.getActiveWorkflow()->getName(), 
        "Engine incorrectly skipped to child workflow before parent was finished!");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_workflow_hierarchy_skip_bug);
    return UNITY_END();
}
