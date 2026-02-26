#ifndef SCALE_LOGIC_H
#define SCALE_LOGIC_H

#include "../Interfaces/ISwitch.h"
#include "../Sensors/Virtual/ManualPumpTimer.h"
#include "../Sensors/Virtual/TaredWeight.h"

/**
 * Coordination module for scale-related machine behavior.
 * Decouples trigger logic (pump switch) from measurement (timer/weight).
 */
class ScaleLogic {
public:
    ScaleLogic(ISwitch* pump, ManualPumpTimer* timer, TaredWeight* weight);

    void update();

private:
    ISwitch* _pump;
    ManualPumpTimer* _timer;
    TaredWeight* _weight;
};

#endif
