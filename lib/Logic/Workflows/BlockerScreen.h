#ifndef BLOCKER_SCREEN_H
#define BLOCKER_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../Interfaces/IBlocker.h"
#include "../../Interfaces/IPainter.h"
class BlockerScreen : public IScreen {
public:
    BlockerScreen(IBlocker* blocker, ISensorRegistry* registry);
    virtual ~BlockerScreen();

    void update() override {}
    bool isDone() const override;
    void paint(IPainter& p) override;
    
    ScreenComposition getComposition() const override {
        return ScreenComposition(1, 1)
            .add(WidgetType::STATUS, _blocker->getTagName());
    }

private:
    IBlocker* _blocker;
    ISensorRegistry* _registry;
};

#endif
