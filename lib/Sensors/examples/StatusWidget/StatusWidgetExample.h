#ifndef STATUS_WIDGET_EXAMPLE_H
#define STATUS_WIDGET_EXAMPLE_H

#include "IExample.h"
#include "StatusWidget.h"

class StatusWidgetExample : public IExample {
public:
    std::string getWidgetName() const override { return "StatusWidget"; }

    std::vector<State> getStates() const override {
        return {
            {"zero", Reading(0.0f, "", "READY", 0, false)},
            {"max", Reading(1.0f, "", "MAXIMUM", 0, false)},
            {"error", Reading(0.0f, "", "SENSOR ERROR", 0, true)}
        };
    }

    lv_obj_t* createWidget(lv_obj_t* parent) override {
        _widget = new StatusWidget();
        return _widget->init(parent, 1, 1);
    }

    void updateWidget(const Reading& reading) override {
        if (_widget) _widget->update(reading);
    }

    void applyTheme(ITheme* theme) override {
        if (_widget) _widget->applyTheme(theme);
    }

private:
    StatusWidget* _widget;
};

#endif
