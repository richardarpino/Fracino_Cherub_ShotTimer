#ifndef WORKFLOW_RUNNING_TRIGGER_H
#define WORKFLOW_RUNNING_TRIGGER_H

#include "../../Interfaces/ITrigger.h"
#include "../../Interfaces/IWorkflow.h"

/**
 * A trigger that is active as long as the associated workflow is NOT finished.
 * Used for one-shot sequences like Startup to ensure they release control when done.
 */
class WorkflowRunningTrigger : public ITrigger {
public:
    WorkflowRunningTrigger(IWorkflow* workflow) : _workflow(workflow) {}
    
    void update() override {}
    
    bool isActive() const override {
        return _workflow && !_workflow->isFinished();
    }

private:
    IWorkflow* _workflow;
};

#endif
