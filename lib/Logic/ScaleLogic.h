#ifndef SCALE_LOGIC_H
#define SCALE_LOGIC_H

#include "../Interfaces/ISwitch.h"
#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "ManualPumpTimer.h"
#include "TaredWeight.h"
#include "BoilerTemperature.h"

/**
 * Coordination module for scale-related machine behavior.
 * SRP: Detects machine events (pump) and orchestrates logical producers.
 * Acts as a Registry Producer: Publishes calculated results to the ISensorRegistry.
 */
class ScaleLogic {
public:
    ScaleLogic(ISwitch* pump, ManualPumpTimer* timer, TaredWeight* weight, 
               BoilerTemperature* temp, ISensorRegistry* registry);

    void update();

private:
    ISwitch* _pump;
    ManualPumpTimer* _timer;
    TaredWeight* _weight;
    BoilerTemperature* _temp;
    ISensorRegistry* _registry;
};

#endif
