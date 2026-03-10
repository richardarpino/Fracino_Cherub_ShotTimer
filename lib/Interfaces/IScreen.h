#ifndef ISCREEN_H
#define ISCREEN_H

#include "SensorTypes.h"

class IPainter;

/**
 * Interface for a visual unit in a Workflow.
 * A Screen owns its Widgets and defines their layout.
 */
class IScreen {
public:
    virtual ~IScreen() {}
    virtual void update() = 0;
    virtual bool isDone() const = 0;
    virtual const char* getName() const = 0;
    virtual const char* getDescription() const = 0;
    virtual const char* getExitCondition() const = 0;
    
    // The Manifest: Subclasses describe their UI here
    virtual ScreenComposition getComposition() const = 0;

    // Incoming Transition Control: -1 = use engine default, 0+ = specific delay in ms
    virtual void setTransitionDelay(int delayMs) {}
    virtual int getTransitionDelay() const { return -1; }

    // Bridge: Calls the painter with our composition
    // Note: Most screens have their own _registry member
    virtual void paint(IPainter& painter) = 0;
};

#endif
