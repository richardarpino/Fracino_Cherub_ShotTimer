#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <vector>
#include "../Themes/IThemeable.h"
#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "../Sensors/Registry/RegistrySwitch.h"
#include "../Themes/ITheme.h"

class ThemeManager {
public:
    ThemeManager(IThemeable* display, ISensorRegistry* registry);

    void addTheme(ITheme* theme);
    void update();

private:
    IThemeable* _display;
    ISensorRegistry* _registry;
    RegistrySwitch<ButtonRightTag> _themeButton;
    std::vector<ITheme*> _themes;
    int _currentIndex;
};

#endif
