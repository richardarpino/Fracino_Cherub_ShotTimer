#ifndef ITHEME_H
#define ITHEME_H

#include <TFT_eSPI.h>

class ITheme {
public:
    virtual uint16_t getBackgroundColor() = 0;
    virtual uint16_t getTextColor() = 0;
    virtual uint16_t getLabelColor() = 0;
    virtual uint16_t getAlertTextColor() = 0;
    virtual uint16_t getAlertBackgroundColor() = 0;
    virtual ~ITheme() {}
};

#endif
