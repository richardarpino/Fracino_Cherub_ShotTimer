#ifndef DEFAULT_THEME_H
#define DEFAULT_THEME_H

#include "ITheme.h"
#include "Colors.h"

class DefaultTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return THEME_BLACK; }
    uint16_t getTextColor() override { return THEME_GREEN; }
    uint16_t getLabelColor() override { return THEME_DARKGREY; }
    uint16_t getAlertTextColor() override { return THEME_WHITE; }
    uint16_t getAlertBackgroundColor() override { return THEME_RED; }
};

#endif
