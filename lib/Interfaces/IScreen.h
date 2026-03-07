#ifndef ISCREEN_H
#define ISCREEN_H

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
    
    // Bridge to new IPainter pattern
    virtual void paint(IPainter& painter) {}
};

#endif
