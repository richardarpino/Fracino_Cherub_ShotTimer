#ifndef SHOT_DISPLAY_H
#define SHOT_DISPLAY_H

#include <TFT_eSPI.h>
#include "../Themes/ITheme.h"

class ShotDisplay {
public:
    ShotDisplay(ITheme* initialTheme);
    void init();
    
    // Theme Management
    void setTheme(ITheme* newTheme);

    // Screens
    void showInfo(const String& topText, const String& bottomText);
    void clearScreen();

    // Updates
    void updateTimer(float currentTime);
    void updateBoilerState(float pressure, float temp);

private:
    TFT_eSPI _tft;
    ITheme* _currentTheme;
    int _lastDisplayedDeci;
    int _lastDisplayedPressureDeci;
    int _lastDisplayedTempDeci;

    void applyTheme(); // Helper to set background/text color generic defaults
};

#endif
