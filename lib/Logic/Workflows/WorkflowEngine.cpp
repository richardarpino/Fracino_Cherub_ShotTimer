#include "WorkflowEngine.h"
#include <algorithm>

#include <Arduino.h>

WorkflowEngine::WorkflowEngine(ISensorRegistry* registry, uint32_t transitionPauseMs)
    : _registry(registry), _rootNode(nullptr), _default(nullptr), _activeWorkflow(nullptr),
      _transitionPauseMs(transitionPauseMs), _currentTransitionPauseMs(0), _lastScreen(nullptr), 
      _transitionStartTime(0), _isTransitioning(false) {}

WorkflowEngine::~WorkflowEngine() {
    if (_rootNode) delete _rootNode;
}

void WorkflowEngine::setRootWorkflow(IWorkflow* root) {
    if (_rootNode) delete _rootNode;
    _rootNode = new WorkflowNode(root);
    if (!_activeWorkflow) _activeWorkflow = root;
}

void WorkflowEngine::setDefaultWorkflow(IWorkflow* defaultWf) {
    _default = defaultWf;
}

void WorkflowEngine::addTriggerWorkflow(IWorkflow* workflow, ITrigger* trigger, int precedence, IWorkflow* parent) {
    if (!_rootNode) return;
    
    WorkflowNode* parentNode = _rootNode;
    if (parent) {
        parentNode = _rootNode->findNode(parent);
    }

    if (parentNode) {
        parentNode->addChild(new WorkflowNode(workflow), trigger, precedence);
    }
}

void WorkflowEngine::addGlobalTrigger(IWorkflow* workflow, ITrigger* trigger, int precedence) {
    _globalTriggers.emplace_back(workflow, trigger, precedence);
    
    // Sort by precedence (highest first)
    std::sort(_globalTriggers.begin(), _globalTriggers.end(), 
        [](const GlobalTrigger& a, const GlobalTrigger& b) {
            return a.precedence > b.precedence;
        });
}

IWorkflow* WorkflowEngine::findNextActiveWorkflow() const {
    const_cast<WorkflowEngine*>(this)->_activeBreadcrumb = "";

    // 1. Check Global Triggers first (Override everything)
    for (auto& gt : _globalTriggers) {
        if (gt.trigger) {
            gt.trigger->update();
            if (gt.trigger->isActive()) {
                const_cast<WorkflowEngine*>(this)->_activeBreadcrumb = "Global > ";
                const_cast<WorkflowEngine*>(this)->_activeBreadcrumb += (gt.workflow ? gt.workflow->getName() : "Unknown");
                return gt.workflow;
            }
        }
    }

    if (!_rootNode) return nullptr;
    
    WorkflowNode* currentNode = _rootNode;
    IWorkflow* result = _rootNode->getWorkflow();

    while (currentNode) {
        IWorkflow* wf = currentNode->getWorkflow();
        result = wf;

        if (!const_cast<WorkflowEngine*>(this)->_activeBreadcrumb.empty()) {
            const_cast<WorkflowEngine*>(this)->_activeBreadcrumb += " > ";
        }
        const_cast<WorkflowEngine*>(this)->_activeBreadcrumb += (wf ? wf->getName() : "Unknown");

        // Check for blockers
        IScreen* screen = wf ? wf->getActiveScreen() : nullptr;
        if (screen && !screen->isDone()) {
            return wf;
        }

        // Evaluate children
        WorkflowNode* nextNode = nullptr;
        int highestPrecedence = -1;

        for (auto& child : currentNode->getChildren()) {
            if (child.trigger) {
                child.trigger->update();
                if (child.trigger->isActive()) {
                    if (child.precedence > highestPrecedence) {
                        nextNode = child.node;
                        highestPrecedence = child.precedence;
                    }
                }
            }
        }

        if (nextNode) {
            currentNode = nextNode;
        } else {
            // Default fallback if root is finished
            if (wf && wf->isFinished() && _default && currentNode == _rootNode) {
                const_cast<WorkflowEngine*>(this)->_activeBreadcrumb = "Default";
                return _default;
            }
            break;
        }
    }
    return result;
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

    IWorkflow* nextActive = findNextActiveWorkflow();

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

const char* WorkflowEngine::getActiveBreadcrumb() const {
    return _activeBreadcrumb.c_str();
}

IScreen* WorkflowEngine::getActiveScreen() const {
    if (_isTransitioning && _lastScreen) return _lastScreen;
    return _activeWorkflow ? _activeWorkflow->getActiveScreen() : nullptr;
}
