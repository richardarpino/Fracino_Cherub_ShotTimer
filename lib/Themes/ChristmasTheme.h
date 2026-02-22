#ifndef CHRISTMAS_THEME_H
#define CHRISTMAS_THEME_H

#include "ITheme.h"
#include "Colors.h"

class ChristmasTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return THEME_RED; }
    uint16_t getTextColor() override { return THEME_WHITE; }
    uint16_t getLabelColor() override { return THEME_GREEN; }
    uint16_t getAlertTextColor() override { return THEME_YELLOW; }
    uint16_t getAlertBackgroundColor() override { return THEME_RED; }
};

#endif
