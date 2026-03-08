#ifndef IPAINTER_H
#define IPAINTER_H

#include "SensorTypes.h"

/**
 * Interface for drawing UI elements.
 * Logic-side IScreen objects call these methods to describe their state.
 */
class IPainter {
public:
    virtual ~IPainter() {}

    // The Generic Manifest-Driven Draw
    virtual void draw(const ScreenComposition& composition, class ISensorRegistry* registry) = 0;
    
    // Legacy support for manual layout control (can be removed later)
    virtual void setLayout(uint8_t cols, uint8_t rows) = 0;
};

#endif
