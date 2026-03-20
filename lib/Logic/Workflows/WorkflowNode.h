#ifndef WORKFLOW_NODE_H
#define WORKFLOW_NODE_H

#include "../../Interfaces/IWorkflow.h"
#include "../../Interfaces/ITrigger.h"
#include <vector>

/**
 * A node in the hierarchical workflow tree.
 * Each node holds a workflow and a list of triggered child nodes.
 */
class WorkflowNode {
public:
    struct TriggeredChild {
        WorkflowNode* node;
        ITrigger* trigger;
        int precedence;

        TriggeredChild(WorkflowNode* n, ITrigger* t, int p) 
            : node(n), trigger(t), precedence(p) {}
    };

    WorkflowNode(IWorkflow* workflow) : _workflow(workflow) {}
    
    ~WorkflowNode() {
        for (auto& child : _children) {
            delete child.node;
        }
    }

    void addChild(WorkflowNode* node, ITrigger* trigger, int precedence) {
        _children.emplace_back(node, trigger, precedence);
    }

    IWorkflow* getWorkflow() const { return _workflow; }
    const std::vector<TriggeredChild>& getChildren() const { return _children; }

    WorkflowNode* findNode(IWorkflow* workflow) {
        if (_workflow == workflow) return this;
        for (auto& child : _children) {
            WorkflowNode* found = child.node->findNode(workflow);
            if (found) return found;
        }
        return nullptr;
    }

private:
    IWorkflow* _workflow;
    std::vector<TriggeredChild> _children;
};

#endif
