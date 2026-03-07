#include "BlockerScreen.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

BlockerScreen::BlockerScreen(IBlocker* blocker) : _blocker(blocker) {
    _layout = new ScreenLayout();
    _layout->setDimensions(1, 1); // Full screen
    _widget = new BlockerWidget(blocker);
    _layout->addWidget(_widget);
}

BlockerScreen::~BlockerScreen() {
    delete _layout;
    // Widget is owned by layout
}

ScreenLayout* BlockerScreen::getLayout() {
    return _layout;
}

void BlockerScreen::update() {
    _layout->update();
}

bool BlockerScreen::isDone() const {
    // isActive() TRUE means unblocked (condition met)
    return _blocker && _blocker->isActive();
}
