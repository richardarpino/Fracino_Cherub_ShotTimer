#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <vector>
#include "ShotDisplay.h"
#include "../Interfaces/ISensor.h"
#include "../Themes/ITheme.h"

class ThemeManager {
public:
    ThemeManager(ShotDisplay* display, IRawSource* buttonSource, unsigned long debounceMs = 250);

    void addTheme(ITheme* theme);
    void update();

private:
    ShotDisplay* _display;
    IRawSource* _buttonSource;
    std::vector<ITheme*> _themes;
    int _currentIndex;
    unsigned long _lastPressTime;
    unsigned long _debounceTime;
};

#endif
