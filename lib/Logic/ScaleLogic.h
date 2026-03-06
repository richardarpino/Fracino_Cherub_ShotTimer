#ifndef SCALE_LOGIC_H
#define SCALE_LOGIC_H

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include "../Sensors/Registry/RegistrySwitch.h"
#include "TaredWeight.h"

/**
 * Coordination module for scale-related machine behavior.
 * SRP: Strictly coordinates Weight/Tare based on pump signal.
 */
class ScaleLogic {
public:
    ScaleLogic(TaredWeight* weight, ISensorRegistry* registry);

    void update();

private:
    RegistrySwitch<PumpReading> _pump;
    TaredWeight* _weight;
    ISensorRegistry* _registry;
};

#endif
