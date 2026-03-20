#ifndef IWORKFLOW_H
#define IWORKFLOW_H

#include "IScreen.h"

/**
 * Interface for a managed sequence of Screens.
 * Handles navigation (next/back) and identifies completion.
 */
class IWorkflow {
public:
    virtual ~IWorkflow() {}

    virtual void addScreen(IScreen* screen) = 0;
    virtual IScreen* getActiveScreen() const = 0;
    
    virtual void update() = 0;
    virtual void next() = 0;
    virtual bool isFinished() const = 0;

    virtual const char* getName() const = 0;
    virtual const char* getDescription() const = 0;

    // Transition Control for the incoming workflow
    virtual int getTransitionPause() const = 0;
};

#endif
