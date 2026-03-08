#include "WorkflowEngine.h"
#include <algorithm>

WorkflowEngine::WorkflowEngine(ISensorRegistry* registry)
    : _registry(registry), _root(nullptr), _default(nullptr), _activeWorkflow(nullptr) {}

void WorkflowEngine::setRootWorkflow(IWorkflow* root) {
    _root = root;
    if (!_activeWorkflow) _activeWorkflow = root;
}

void WorkflowEngine::setDefaultWorkflow(IWorkflow* defaultWf) {
    _default = defaultWf;
}

void WorkflowEngine::addTriggerWorkflow(IWorkflow* workflow, TriggerType trigger, int precedence) {
    _triggeredWorkflows.emplace_back(workflow, trigger, precedence, _registry);
    
    // Sort by precedence (highest first)
    std::sort(_triggeredWorkflows.begin(), _triggeredWorkflows.end(), 
        [](const TriggeredWorkflow& a, const TriggeredWorkflow& b) {
            return a.precedence > b.precedence;
        });
}

void WorkflowEngine::update() {
    if (!_registry) return;

    if (_activeWorkflow) {
        _activeWorkflow->update();
    }

    IWorkflow* nextActive = _root;
    
    // If root is done, fall back to default
    if (_root && _root->isFinished() && _default) {
        nextActive = _default;
    }

    int highestMatchedPrecedence = -1;

    for (auto& tw : _triggeredWorkflows) {
        bool isActive = false;
        if (tw.type == TriggerType::BUTTON_LEFT) {
            tw.leftButton.update();
            isActive = tw.leftButton.isActive();
        } else if (tw.type == TriggerType::PUMP) {
            tw.pump.update();
            isActive = tw.pump.isActive();
        }

        if (isActive) {
            if (tw.precedence > highestMatchedPrecedence) {
                nextActive = tw.workflow;
                highestMatchedPrecedence = tw.precedence;
            }
        }
    }

    _activeWorkflow = nextActive;
}

IWorkflow* WorkflowEngine::getActiveWorkflow() const {
    return _activeWorkflow;
}

IScreen* WorkflowEngine::getActiveScreen() const {
    return _activeWorkflow ? _activeWorkflow->getActiveScreen() : nullptr;
}
