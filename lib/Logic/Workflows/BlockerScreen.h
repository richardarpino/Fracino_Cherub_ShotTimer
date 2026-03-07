#ifndef BLOCKER_SCREEN_H
#define BLOCKER_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../Interfaces/IBlocker.h"
#include "../../UI/BlockerWidget.h"

class BlockerScreen : public IScreen {
public:
    BlockerScreen(IBlocker* blocker);
    virtual ~BlockerScreen();

    ScreenLayout* getLayout() override;
    void update() override;
    bool isDone() const override;

private:
    IBlocker* _blocker;
    ScreenLayout* _layout;
    BlockerWidget* _widget;
};

#endif
