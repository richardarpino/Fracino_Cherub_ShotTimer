#ifndef STATUS_WIDGET_H
#define STATUS_WIDGET_H

#include <lvgl.h>
#include <Arduino.h>
#include "IWidget.h"
#include "../Interfaces/ISensorRegistry.h"

/**
 * Base class for Status UI logic.
 */
class StatusWidgetBase : public IWidget {
public:
    StatusWidgetBase();
    virtual ~StatusWidgetBase() = default;

    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override;
    void update(const Reading& reading) override;
    void applyTheme(ITheme* theme) override;
    
    bool isStatusWidget() override { return true; }
    void setText(const char* text);

protected:
    lv_obj_t* _container;
    lv_obj_t* _label;
    unsigned long _messageTimeout;
    
    lv_color_t _bgColor, _textColor, _errorColor, _alertBgColor;
};

/**
 * Templated Registry-Aware Status Widget
 */
template<typename T = void>
class StatusWidget : public StatusWidgetBase {
public:
    StatusWidget(ISensorRegistry* registry = nullptr) : _registry(registry) {}

    void setRegistry(ISensorRegistry* registry) override {
        _registry = registry;
    }

    void refresh() override {
        if (_registry && millis() > _messageTimeout) {
            update(_registry->getLatest<T>());
        }
    }

private:
    ISensorRegistry* _registry;
};

/**
 * Legacy Pointer-Based Status Widget
 */
template<>
class StatusWidget<void> : public StatusWidgetBase {
public:
    StatusWidget(ISensor* sensor) : _sensor(sensor) {}

    void refresh() override {
        if (_sensor && millis() > _messageTimeout) {
            update(_sensor->getReading());
        }
    }

private:
    ISensor* _sensor;
};

#endif
