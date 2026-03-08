#include "WorkflowEngine.h"
#include <algorithm>

#include <Arduino.h>

WorkflowEngine::WorkflowEngine(ISensorRegistry* registry, uint32_t transitionPauseMs)
    : _registry(registry), _root(nullptr), _default(nullptr), _activeWorkflow(nullptr),
      _transitionPauseMs(transitionPauseMs), _currentTransitionPauseMs(0), _lastScreen(nullptr), 
      _transitionStartTime(0), _isTransitioning(false) {}

void WorkflowEngine::setRootWorkflow(IWorkflow* root) {
    _root = root;
    if (!_activeWorkflow) _activeWorkflow = root;
}

void WorkflowEngine::setDefaultWorkflow(IWorkflow* defaultWf) {
    _default = defaultWf;
}

void WorkflowEngine::addTriggerWorkflow(IWorkflow* workflow, ITrigger* trigger, int precedence) {
    _triggeredWorkflows.emplace_back(workflow, trigger, precedence);
    
    // Sort by precedence (highest first)
    std::sort(_triggeredWorkflows.begin(), _triggeredWorkflows.end(), 
        [](const TriggeredWorkflow& a, const TriggeredWorkflow& b) {
            return a.precedence > b.precedence;
        });
}

void WorkflowEngine::update() {
    if (_isTransitioning) {
        if (millis() - _transitionStartTime >= _currentTransitionPauseMs) {
            _isTransitioning = false;
        } else {
            return; // Stay on previous screen visually
        }
    }

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
        if (tw.trigger) {
            tw.trigger->update();
            if (tw.trigger->isActive()) {
                if (tw.precedence > highestMatchedPrecedence) {
                    nextActive = tw.workflow;
                    highestMatchedPrecedence = tw.precedence;
                }
            }
        }
    }

    // Detect Transition
    if (nextActive != _activeWorkflow) {
        // Determine pause duration: check incoming workflow override, else use logic default
        uint32_t pause = _transitionPauseMs;
        if (nextActive) {
            int overridePause = nextActive->getTransitionPause();
            if (overridePause >= 0) pause = (uint32_t)overridePause;
        }

        if (pause > 0 && _activeWorkflow) {
            _lastScreen = _activeWorkflow->getActiveScreen();
            _isTransitioning = true;
            _transitionStartTime = millis();
            _currentTransitionPauseMs = pause;
            _activeWorkflow = nextActive; // Step the pointer immediately
            
            // If the requested pause is 0, we can skip the transition state entirely
            if (pause == 0) {
                _isTransitioning = false;
            }
        } else {
             _activeWorkflow = nextActive;
        }
    }
}

IWorkflow* WorkflowEngine::getActiveWorkflow() const {
    return _activeWorkflow;
}

IScreen* WorkflowEngine::getActiveScreen() const {
    if (_isTransitioning && _lastScreen) return _lastScreen;
    return _activeWorkflow ? _activeWorkflow->getActiveScreen() : nullptr;
}
