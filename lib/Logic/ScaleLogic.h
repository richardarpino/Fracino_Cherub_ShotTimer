#ifndef SCALE_LOGIC_H
#define SCALE_LOGIC_H

#include "../Interfaces/ISwitch.h"
#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "TaredWeight.h"

/**
 * Coordination module for scale-related machine behavior.
 * SRP: Strictly coordinates Weight/Tare based on pump signal.
 */
class ScaleLogic {
public:
    ScaleLogic(ISwitch* pump, TaredWeight* weight, ISensorRegistry* registry);

    void update();

private:
    ISwitch* _pump;
    TaredWeight* _weight;
    ISensorRegistry* _registry;
};

#endif
