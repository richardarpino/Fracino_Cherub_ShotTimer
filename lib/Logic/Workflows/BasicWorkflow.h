#ifndef BASIC_WORKFLOW_H
#define BASIC_WORKFLOW_H

#include "../../Interfaces/IWorkflow.h"
#include <vector>

/**
 * A simple sequential workflow implementation.
 */
class BasicWorkflow : public IWorkflow {
public:
    BasicWorkflow();
    
    void addScreen(IScreen* screen) override;
    IScreen* getActiveScreen() const override;
    
    void update() override;
    void next() override;
    bool isFinished() const override;
    int getTransitionPause() const override;

private:
    std::vector<IScreen*> _screens;
    size_t _currentIndex;
    bool _isFinished;
};

#endif
