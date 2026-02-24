#ifndef IBLOCKER_H
#define IBLOCKER_H

#include "ISwitch.h"
#include <Arduino.h>

/**
 * Interface for a process that blocks the UI/system flow until a condition is met.
 * Extends ISwitch: isActive() means the gate is OPEN (condition met).
 */
class IBlocker : public ISwitch {
public:
    virtual ~IBlocker() {}

    // Status message for the UI (e.g., "Connecting...", "Warming Up...")
    virtual String getStatusMessage() const = 0;

    // Progress percentage (0.0-100.0). Returns -1.0 if indeterminate.
    virtual float getProgress() const = 0;

    // Returns true if the process encountered a terminal error.
    virtual bool isFailed() const = 0;
};

#endif
