#ifndef ISWITCH_H
#define ISWITCH_H

/**
 * Interface for a logical binary state with edge detection.
 * Used to decouple discrete event logic (Start/Stop) from continuous sensing.
 */
class ISwitch {
public:
    virtual ~ISwitch() {}
    
    virtual void update() = 0;
    virtual bool isActive() const = 0;
    virtual bool justStarted() const = 0;
    virtual bool justStopped() const = 0;
};

#endif
