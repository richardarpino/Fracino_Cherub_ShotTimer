#ifndef IPAINTER_H
#define IPAINTER_H

#include <stdint.h>

/**
 * Interface for drawing UI elements.
 * Logic-side IScreen objects call these methods to describe their state.
 */
class IPainter {
public:
    virtual ~IPainter() {}

    virtual void drawBlocker(const char* title, const char* message, float progress, bool isAlert) = 0;
    virtual void drawGauge(const char* label, float value, float min, float max) = 0;
    virtual void drawStatus(const char* label, const char* value, bool isAlert) = 0;
    virtual void drawDashboard(class ISensorRegistry* registry) = 0;
    virtual void setLayout(uint8_t cols, uint8_t rows) = 0;
};

#endif
