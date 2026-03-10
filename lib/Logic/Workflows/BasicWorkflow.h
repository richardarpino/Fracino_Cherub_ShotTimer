#ifndef BASIC_WORKFLOW_H
#define BASIC_WORKFLOW_H

#include "../../Interfaces/IWorkflow.h"
#include <vector>

/**
 * A simple sequential workflow implementation.
 */
class BasicWorkflow : public IWorkflow {
public:
    BasicWorkflow(const char* name = "Unnamed Workflow", const char* description = "No description provided");
    virtual ~BasicWorkflow();
    
    void addScreen(IScreen* screen) override;
    IScreen* getActiveScreen() const override;
    
    void update() override;
    void next() override;
    bool isFinished() const override;

    const char* getName() const override { return _name; }
    const char* getDescription() const override { return _description; }
    int getTransitionPause() const override;

private:
    const char* _name;
    const char* _description;
    std::vector<IScreen*> _screens;
    size_t _currentIndex;
    bool _isFinished;
};

#endif
