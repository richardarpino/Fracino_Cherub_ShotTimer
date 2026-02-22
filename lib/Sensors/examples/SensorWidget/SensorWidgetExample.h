#ifndef SENSOR_WIDGET_EXAMPLE_H
#define SENSOR_WIDGET_EXAMPLE_H

#include "IExample.h"
#include "SensorWidget.h"

class SensorWidgetExample : public IExample {
public:
    std::string getWidgetName() const override { return "SensorWidget"; }

    std::vector<State> getStates() const override {
        return {
            {"zero", Reading(0.0f, "bar", "0.0", 1, false)},
            {"max", Reading(12.55f, "bar", "12.6", 1, false)},
            {"error", Reading(0.0f, "bar", "ERR", 1, true)}
        };
    }

    lv_obj_t* createWidget(lv_obj_t* parent) override {
        _widget = new SensorWidget();
        return _widget->init(parent, 1, 1);
    }

    void updateWidget(const Reading& reading) override {
        if (_widget) _widget->update(reading);
    }

    void applyTheme(ITheme* theme) override {
        if (_widget) _widget->applyTheme(theme);
    }

private:
    SensorWidget* _widget;
};

#endif
