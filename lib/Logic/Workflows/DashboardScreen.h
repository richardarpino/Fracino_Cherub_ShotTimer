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
    
    ScreenComposition getComposition() const override {
        return ScreenComposition(2, 2)
            .add(WidgetType::GAUGE, BoilerPressureReading::NAME)
            .add(WidgetType::SENSOR, BoilerTempReading::NAME)
            .add(WidgetType::SENSOR, HeatingCycleReading::NAME)
            .add(WidgetType::SENSOR, LastValidShotReading::NAME);
    }

private:
    ISensorRegistry* _registry;
};

#endif
