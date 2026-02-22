#ifndef STATUS_WIDGET_EXAMPLE_H
#define STATUS_WIDGET_EXAMPLE_H

#include "IExample.h"
#include "StatusWidget.h"

EXAMPLE_FOR(StatusWidget)
    std::vector<State> getStates() const override {
        return {
            {"zero", Reading(0.0f, "", "READY", 0, false)},
            {"max", Reading(1.0f, "", "MAXIMUM", 0, false)},
            {"error", Reading(0.0f, "", "SENSOR ERROR", 0, true)}
        };
    }
};

#endif
