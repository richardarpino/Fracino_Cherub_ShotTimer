#ifndef IBLOCKER_H
#define IBLOCKER_H

#include "ISwitch.h"
#include "SensorTypes.h"

/**
 * Interface for a process that blocks the UI/system flow until a condition is met.
 * Extends ISwitch: isActive() means the gate is OPEN (condition met).
 */
class IBlocker : public ISwitch {
public:
    virtual ~IBlocker() {}

    // Get the full structured status message of the blocker
    virtual StatusMessage getStatus() const = 0;
};

#endif
