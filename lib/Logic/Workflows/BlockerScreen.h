#ifndef BLOCKER_SCREEN_H
#define BLOCKER_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../Interfaces/IBlocker.h"
#include "../../Interfaces/IPainter.h"
class BlockerScreen : public IScreen {
public:
    BlockerScreen(IBlocker* blocker);
    virtual ~BlockerScreen();

    void update() override {}
    bool isDone() const override;
    void paint(IPainter& p) override;

private:
    IBlocker* _blocker;
};

#endif
