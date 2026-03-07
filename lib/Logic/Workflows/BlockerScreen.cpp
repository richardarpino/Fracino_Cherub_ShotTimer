#include "BlockerScreen.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

BlockerScreen::BlockerScreen(IBlocker* blocker) : _blocker(blocker) {
}

BlockerScreen::~BlockerScreen() {
}

bool BlockerScreen::isDone() const {
    // isActive() TRUE means unblocked (condition met)
    return _blocker && _blocker->isActive();
}

void BlockerScreen::paint(IPainter& p) {
    if (_blocker) {
        StatusMessage status = _blocker->getStatus();
        p.drawBlocker(status.title, status.message, status.progress, status.isFailed);
    }
}
