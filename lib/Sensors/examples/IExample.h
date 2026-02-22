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

template <typename T>
class BaseWidgetExample : public IExample {
public:
    BaseWidgetExample(const std::string& name) : _name(name) {}
    virtual ~BaseWidgetExample() {
        if (_widget) delete _widget;
    }

    std::string getWidgetName() const override { return _name; }

    lv_obj_t* createWidget(lv_obj_t* parent) override {
        _widget = new T();
        return _widget->init(parent, 1, 1);
    }

    void updateWidget(const Reading& reading) override {
        if (_widget) _widget->update(reading);
    }

    void applyTheme(ITheme* theme) override {
        if (_widget) _widget->applyTheme(theme);
    }

protected:
    T* _widget = nullptr;
    std::string _name;
};

#define EXAMPLE_FOR(WidgetClass) \
class WidgetClass##Example : public BaseWidgetExample<WidgetClass> { \
public: \
    WidgetClass##Example() : BaseWidgetExample(#WidgetClass) {}

#endif
