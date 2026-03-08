#ifndef SHOT_SCREEN_H
#define SHOT_SCREEN_H

#include "../../Interfaces/IScreen.h"
#include "../../Interfaces/IPainter.h"
#include "../SensorDispatcher.h"

class ShotScreen : public IScreen {
public:
    ShotScreen(ISensorRegistry* registry);
    virtual ~ShotScreen();

    void update() override;
    bool isDone() const override;
    void paint(IPainter& p) override;
    
    ScreenComposition getComposition() const override {
        return ScreenComposition(2, 1)
            .add(WidgetType::SENSOR, LastValidShotReading::NAME)
            .add(WidgetType::SENSOR, ShotTimeReading::NAME);
    }

private:
    ISensorRegistry* _registry;
};

#endif
