#ifndef WORKFLOW_ENGINE_H
#define WORKFLOW_ENGINE_H

#include "../../Interfaces/IWorkflow.h"
#include "../../Interfaces/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"
#include "../Registry/RegistrySwitch.h"
#include <map>
#include <vector>

enum class TriggerType {
    NONE,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    PRESSURE_THRESHOLD
};

/**
 * Manages the transition between multiple Workflows.
 * Uses precedence to decide which workflow is currently active.
 */
class WorkflowEngine {
public:
    WorkflowEngine(ISensorRegistry* registry);

    void setRootWorkflow(IWorkflow* root);
    void addTriggerWorkflow(IWorkflow* workflow, TriggerType trigger, int precedence);

    void update();
    
    IWorkflow* getActiveWorkflow() const;
    IScreen* getActiveScreen() const;

private:
    struct TriggeredWorkflow {
        IWorkflow* workflow;
        TriggerType type;
        int precedence;
        RegistrySwitch<ButtonLeftReading> leftButton; // Simplification for now
        
        TriggeredWorkflow(IWorkflow* w, TriggerType t, int p, ISensorRegistry* r) 
            : workflow(w), type(t), precedence(p), leftButton(r) {}
    };

    ISensorRegistry* _registry;
    IWorkflow* _root;
    std::vector<TriggeredWorkflow> _triggeredWorkflows;
    IWorkflow* _activeWorkflow;
};

#endif
