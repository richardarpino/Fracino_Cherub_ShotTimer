#ifndef SENSOR_WIDGET_EXAMPLE_H
#define SENSOR_WIDGET_EXAMPLE_H

#include "IExample.h"
#include "SensorWidget.h"

EXAMPLE_FOR(SensorWidget)
    std::vector<State> getStates() const override {
        return {
            {"zero", Reading(0.0f, "bar", "0.0", 1, false)},
            {"max", Reading(12.55f, "bar", "12.6", 1, false)},
            {"error", Reading(0.0f, "bar", "ERR", 1, true)}
        };
    }
};

#endif
