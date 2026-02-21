#ifndef SCALE_LOGIC_H
#define SCALE_LOGIC_H

#include "../Interfaces/ISwitch.h"
#include "../Sensors/Hardware/ShotTimer.h"
#include "../Sensors/Virtual/TaredWeight.h"

/**
 * Coordination module for scale-related machine behavior.
 * Decouples trigger logic (pump switch) from measurement (timer/weight).
 */
class ScaleLogic {
public:
    ScaleLogic(ISwitch* pump, ShotTimer* timer, TaredWeight* weight);

    void update();

private:
    ISwitch* _pump;
    ShotTimer* _timer;
    TaredWeight* _weight;
};

#endif
