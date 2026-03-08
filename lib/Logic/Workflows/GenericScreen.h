#ifndef GENERIC_SCREEN_H
#define GENERIC_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../Interfaces/IPainter.h"

/**
 * A data-driven screen that renders based on a ScreenComposition manifest.
 * Eliminates the need for specialized subclasses for every screen layout.
 */
#include "../../Interfaces/IBlocker.h"

/**
 * A data-driven screen that renders based on a ScreenComposition manifest.
 * Eliminates the need for specialized subclasses for every screen layout.
 */
class GenericScreen : public IScreen {
public:
    GenericScreen(const ScreenComposition& composition, class ISensorRegistry* registry, IBlocker* blocker = nullptr)
        : _composition(composition), _registry(registry), _blocker(blocker), _transitionDelay(-1) {}

    void setTransitionDelay(int delayMs) { _transitionDelay = delayMs; }
    int getTransitionDelay() const override { return _transitionDelay; }

    void update() override {
        if (_blocker) _blocker->update();
    }
    
    bool isDone() const override {
        // If we have a blocker, we are done when the blocker is active (finished)
        return _blocker && _blocker->isActive();
    }

    void paint(IPainter& p) override {
        p.draw(_composition, _registry);
    }

    ScreenComposition getComposition() const override {
        return _composition;
    }

private:
    ScreenComposition _composition;
    class ISensorRegistry* _registry;
    IBlocker* _blocker;
    int _transitionDelay;
};

#endif
