#ifndef CANDY_THEME_H
#define CANDY_THEME_H

#include "ITheme.h"
#include "Colors.h"

class CandyTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return THEME_WHITE; }
    uint16_t getTextColor() override { return THEME_MAGENTA; }
    uint16_t getLabelColor() override { return THEME_PURPLE; }
    // In candy theme, alert is maybe bright orange
    uint16_t getAlertTextColor() override { return THEME_RED; }
    uint16_t getAlertBackgroundColor() override { return THEME_ORANGE; }
};

#endif
