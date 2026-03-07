#ifndef DASHBOARD_SCREEN_H
#define DASHBOARD_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../Interfaces/IPainter.h"
#include "../../Interfaces/SensorTags.h"

class DashboardScreen : public IScreen {
public:
    DashboardScreen(ISensorRegistry* registry);
    virtual ~DashboardScreen();

    void update() override {}
    bool isDone() const override { return false; }
    void paint(IPainter& p) override;

private:
    ISensorRegistry* _registry;
};

#endif
