#ifndef ITRIGGER_FACTORY_H
#define ITRIGGER_FACTORY_H

#include "ITrigger.h"

/**
 * Interface for creating logic triggers.
 * Decouples trigger instantiation from workflow management.
 */
class ITriggerFactory {
public:
    virtual ~ITriggerFactory() {}

    /**
     * Creates a trigger from a string identifier (e.g. "PumpSwitch", "BoilerTemp > 90").
     */
    virtual ITrigger* createTrigger(const char* triggerTag, class ISensorRegistry* registry) = 0;
};

#endif
