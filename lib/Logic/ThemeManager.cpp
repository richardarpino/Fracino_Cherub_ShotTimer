#include "ThemeManager.h"

ThemeManager::ThemeManager(IThemeable* display, ISensorRegistry* registry)
    : _display(display), _registry(registry), _themeButton(registry), _currentIndex(0) {}

void ThemeManager::addTheme(ITheme* theme) {
    if (_themes.empty() && _display) {
        _display->setTheme(theme);
    }
    _themes.push_back(theme);
}

void ThemeManager::update() {
    if (!_display || _themes.empty() || !_registry) return;

    _themeButton.update();
    if (_themeButton.justStarted()) {
        _currentIndex = (_currentIndex + 1) % _themes.size();
        _display->setTheme(_themes[_currentIndex]);
    }
}
