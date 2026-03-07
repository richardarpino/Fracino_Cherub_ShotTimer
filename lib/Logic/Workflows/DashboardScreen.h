#ifndef DASHBOARD_SCREEN_H
#define DASHBOARD_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../UI/SensorWidget.h"
#include "../../Interfaces/SensorTags.h"

class DashboardScreen : public IScreen {
public:
    DashboardScreen(ISensorRegistry* registry);
    virtual ~DashboardScreen();

    ScreenLayout* getLayout() override;
    void update() override;
    bool isDone() const override { return false; } // Dashboard stays forever unless triggered

private:
    ScreenLayout* _layout;
};

#endif
