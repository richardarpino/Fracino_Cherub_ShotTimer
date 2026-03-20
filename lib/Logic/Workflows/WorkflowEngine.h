#ifndef WORKFLOW_ENGINE_H
#define WORKFLOW_ENGINE_H

#include "../../Interfaces/IWorkflow.h"
#include "../../Interfaces/ITrigger.h"
#include <vector>

/**
 * Manages the transition between multiple Workflows.
 * Uses precedence to decide which workflow is currently active.
 */
#include "../../lib/Logic/Workflows/WorkflowNode.h"

class WorkflowEngine {
public:
    WorkflowEngine(class ISensorRegistry* registry, uint32_t transitionPauseMs = 0);
    ~WorkflowEngine();

    void setRootWorkflow(IWorkflow* root);
    void setDefaultWorkflow(IWorkflow* defaultWf);
    void addTriggerWorkflow(IWorkflow* workflow, ITrigger* trigger, int precedence, IWorkflow* parent = nullptr);
    void addGlobalTrigger(IWorkflow* workflow, ITrigger* trigger, int precedence);

    void update();
    
    IWorkflow* getActiveWorkflow() const;
    IScreen* getActiveScreen() const;
    const char* getActiveBreadcrumb() const;

private:
    struct GlobalTrigger {
        IWorkflow* workflow;
        ITrigger* trigger;
        int precedence;
        
        GlobalTrigger(IWorkflow* w, ITrigger* t, int p) 
            : workflow(w), trigger(t), precedence(p) {}
    };

    IWorkflow* findNextActiveWorkflow() const;

    class ISensorRegistry* _registry;
    WorkflowNode* _rootNode;
    std::vector<GlobalTrigger> _globalTriggers;
    IWorkflow* _default; // Keep for now as a fallback
    IWorkflow* _activeWorkflow;
    std::string _activeBreadcrumb;

    // Transition Pause Logic
    uint32_t _transitionPauseMs;
    uint32_t _currentTransitionPauseMs;
    IScreen* _lastScreen;
    unsigned long _transitionStartTime;
    bool _isTransitioning;
};

#endif
