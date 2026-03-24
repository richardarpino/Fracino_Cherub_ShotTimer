#include <unity.h>
#include "Logic/Workflows/WorkflowEngine.h"
#include "Logic/Workflows/WorkflowEngine.cpp"
#include "Logic/Workflows/BasicWorkflow.h"
#include "Logic/Workflows/BasicWorkflow.cpp"
#include "Logic/Workflows/WorkflowNode.h"
#include "Logic/Triggers/LogicalTriggers.h"
#include "Logic/Triggers/DelayedTrigger.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"

// Use BasicWorkflow instead of custom Mock to ensure full implementation

class AlwaysTrigger : public ITrigger {
public:
    void update() override {}
    bool isActive() const override { return true; }
};

class SimpleTrigger : public ITrigger {
public:
    bool state = false;
    void update() override {}
    bool isActive() const override { return state; }
};

void test_boot_regression_repro() {
    WorkflowEngine engine(nullptr, 0);

    BasicWorkflow rootWf("Root", "");
    BasicWorkflow startupWf("Startup", "");
    BasicWorkflow dashboardWf("Dashboard", "");
    BasicWorkflow shotWf("Shot", "");

    engine.setRootWorkflow(&rootWf);

    SimpleTrigger startupTrigger;
    startupTrigger.state = true;

    AlwaysTrigger alwaysTrigger;

    SimpleTrigger pumpTrigger;
    pumpTrigger.state = false; // Pump is OFF at boot
    
    // The Suspect: DelayedTrigger at T=0
    setHardwareTime(0); 
    DelayedTrigger shotSummaryTrigger(&pumpTrigger, 10000);

    engine.addTriggerWorkflow(&startupWf, &startupTrigger, 10, &rootWf);
    engine.addTriggerWorkflow(&dashboardWf, &alwaysTrigger, 1, &rootWf);
    engine.addTriggerWorkflow(&shotWf, &shotSummaryTrigger, 100, &dashboardWf);

    engine.update();

    IWorkflow* active = engine.getActiveWorkflow();
    
    // Log the active workflow name
    printf("Active at T=0: %s\n", active ? active->getName() : "NULL");
    
    // THE EXPECTATION: Startup should be active.
    // If it's Shot, we found the bug.
    TEST_ASSERT_EQUAL_PTR(&startupWf, active);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_boot_regression_repro);
    return UNITY_END();
}
