#ifndef CANDY_THEME_H
#define CANDY_THEME_H

#include "ITheme.h"

class CandyTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return TFT_WHITE; }
    uint16_t getTextColor() override { return TFT_MAGENTA; }
    uint16_t getLabelColor() override { return TFT_PURPLE; }
    // In candy theme, alert is maybe bright orange
    uint16_t getAlertTextColor() override { return TFT_WHITE; }
    uint16_t getAlertBackgroundColor() override { return TFT_ORANGE; }
};

#endif
