#include "ThemeManager.h"

ThemeManager::ThemeManager(IThemeable* display, IRawSource* buttonSource, unsigned long debounceMs)
    : _display(display), _buttonSource(buttonSource), _currentIndex(0), _lastPressTime(0), _debounceTime(debounceMs) {}

void ThemeManager::addTheme(ITheme* theme) {
    if (_themes.empty() && _display) {
        _display->setTheme(theme);
    }
    _themes.push_back(theme);
}

void ThemeManager::update() {
    if (!_buttonSource || !_display || _themes.empty()) return;

    RawReading raw = _buttonSource->read();
    unsigned long now = raw.timestamp;

    // Active LOW logic for button
    if (raw.value == LOW) {
        if (now - _lastPressTime > _debounceTime) {
            _currentIndex = (_currentIndex + 1) % _themes.size();
            _display->setTheme(_themes[_currentIndex]);
            _lastPressTime = now;
        }
    }
}
