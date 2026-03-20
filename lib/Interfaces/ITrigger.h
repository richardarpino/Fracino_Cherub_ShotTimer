#ifndef ITRIGGER_H
#define ITRIGGER_H

/**
 * Interface for a polymorphic trigger.
 * Used by the WorkflowEngine to evaluate transitions.
 */
class ITrigger {
public:
    virtual ~ITrigger() {}
    virtual void update() = 0;
    virtual bool isActive() const = 0;
};

#endif
