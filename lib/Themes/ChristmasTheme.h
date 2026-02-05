#ifndef CHRISTMAS_THEME_H
#define CHRISTMAS_THEME_H

#include "ITheme.h"

class ChristmasTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return TFT_RED; }
    uint16_t getTextColor() override { return TFT_WHITE; }
    uint16_t getLabelColor() override { return TFT_GREEN; }
    uint16_t getAlertTextColor() override { return TFT_RED; }
    uint16_t getAlertBackgroundColor() override { return TFT_WHITE; }
};

#endif
