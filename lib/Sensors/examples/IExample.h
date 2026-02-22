#ifndef IEXAMPLE_H
#define IEXAMPLE_H

#include <lvgl.h>
#include <string>
#include <vector>
#include "Interfaces/ISensor.h"

/**
 * Interface for widget examples that can be rendered in the simulator.
 */
class IExample {
public:
    struct State {
        std::string name;
        Reading reading;
    };

    virtual ~IExample() {}
    virtual std::string getWidgetName() const = 0;
    virtual std::vector<State> getStates() const = 0;
    virtual lv_obj_t* createWidget(lv_obj_t* parent) = 0;
    virtual void updateWidget(const Reading& reading) = 0;
    virtual void applyTheme(ITheme* theme) = 0;
};

#endif
