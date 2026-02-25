#ifndef IBLOCKER_H
#define IBLOCKER_H

#include "ISwitch.h"
#include <Arduino.h>

/**
 * Structured status for a blocker.
 */
struct BlockerStatus {
    String title;    // e.g., "Warming Up..."
    String message;  // e.g., "Heating Cycle 2, currently 0.8bar"
    float progress;  // 0.0 - 100.0, -1.0 for indeterminate
    bool isFailed;

    BlockerStatus(String t = "", String m = "", float p = -1.0f, bool f = false) 
        : title(t), message(m), progress(p), isFailed(f) {}
};

/**
 * Interface for a process that blocks the UI/system flow until a condition is met.
 * Extends ISwitch: isActive() means the gate is OPEN (condition met).
 */
class IBlocker : public ISwitch {
public:
    virtual ~IBlocker() {}

    // Get the full structured status of the blocker
    virtual BlockerStatus getStatus() const = 0;
};

#endif
