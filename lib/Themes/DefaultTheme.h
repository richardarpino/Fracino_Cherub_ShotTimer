#ifndef DEFAULT_THEME_H
#define DEFAULT_THEME_H

#include "ITheme.h"

class DefaultTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return TFT_BLACK; }
    uint16_t getTextColor() override { return TFT_GREEN; }
    uint16_t getLabelColor() override { return TFT_DARKGREY; }
    uint16_t getAlertTextColor() override { return TFT_WHITE; }
    uint16_t getAlertBackgroundColor() override { return TFT_RED; }
};

#endif
