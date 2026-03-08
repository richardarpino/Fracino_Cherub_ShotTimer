#include "BlockerScreen.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

BlockerScreen::BlockerScreen(IBlocker* blocker, ISensorRegistry* registry) 
    : _blocker(blocker), _registry(registry) {
}

BlockerScreen::~BlockerScreen() {
}

void BlockerScreen::paint(IPainter& p) {
    p.draw(getComposition(), _registry);
}

bool BlockerScreen::isDone() const {
    // isActive() TRUE means unblocked (condition met)
    return _blocker && _blocker->isActive();
}
