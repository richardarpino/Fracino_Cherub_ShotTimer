#ifndef WORKFLOW_ENGINE_H
#define WORKFLOW_ENGINE_H

#include "../../Interfaces/IWorkflow.h"
#include "../../Interfaces/ITrigger.h"
#include <vector>

/**
 * Manages the transition between multiple Workflows.
 * Uses precedence to decide which workflow is currently active.
 */
class WorkflowEngine {
public:
    WorkflowEngine(class ISensorRegistry* registry, uint32_t transitionPauseMs = 0);

    void setRootWorkflow(IWorkflow* root);
    void setDefaultWorkflow(IWorkflow* defaultWf);
    void addTriggerWorkflow(IWorkflow* workflow, ITrigger* trigger, int precedence);

    void update();
    
    IWorkflow* getActiveWorkflow() const;
    IScreen* getActiveScreen() const;

private:
    struct TriggeredWorkflow {
        IWorkflow* workflow;
        ITrigger* trigger;
        int precedence;
        
        TriggeredWorkflow(IWorkflow* w, ITrigger* t, int p) 
            : workflow(w), trigger(t), precedence(p) {}
    };

    class ISensorRegistry* _registry;
    IWorkflow* _root;
    IWorkflow* _default;
    std::vector<TriggeredWorkflow> _triggeredWorkflows;
    IWorkflow* _activeWorkflow;

    // Transition Pause Logic
    uint32_t _transitionPauseMs;
    IScreen* _lastScreen;
    unsigned long _transitionStartTime;
    bool _isTransitioning;
};

#endif
