#include "BasicWorkflow.h"

BasicWorkflow::BasicWorkflow() : _currentIndex(0), _isFinished(false) {}

void BasicWorkflow::addScreen(IScreen* screen) {
    if (screen) {
        _screens.push_back(screen);
    }
}

IScreen* BasicWorkflow::getActiveScreen() const {
    if (_currentIndex < _screens.size()) {
        return _screens[_currentIndex];
    }
    return nullptr;
}

void BasicWorkflow::update() {
    if (_isFinished || _screens.empty()) return;

    IScreen* active = getActiveScreen();
    if (active) {
        active->update();
        if (active->isDone()) {
            next();
        }
    }
}

void BasicWorkflow::next() {
    if (_screens.empty()) return;

    if (_currentIndex < _screens.size() - 1) {
        _currentIndex++;
    } else {
        _isFinished = true;
    }
}

bool BasicWorkflow::isFinished() const {
    return _isFinished;
}
