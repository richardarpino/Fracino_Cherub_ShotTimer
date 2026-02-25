#ifndef SENSOR_WIDGET_H
#define SENSOR_WIDGET_H

#include <lvgl.h>
#include "IWidget.h"
#include "../Interfaces/ISensorRegistry.h"

/**
 * Base class for all reading-based widgets.
 * Handles styling, LVGL object management, and the generic update(Reading) logic.
 * This prevents template code bloat by keeping UI logic in a concrete class.
 */
class SensorWidgetBase : public IWidget {
public:
    SensorWidgetBase();
    virtual ~SensorWidgetBase() = default;

    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override;
    void update(const Reading& reading) override;
    void applyTheme(ITheme* theme) override;

protected:
    lv_obj_t* _container;
    lv_obj_t* _value_label;
    lv_obj_t* _unit_label;
    
    // Theme colors
    lv_color_t _bgColor;
    lv_color_t _textColor;
    lv_color_t _labelColor;
    lv_color_t _errorColor;
    lv_color_t _alertBgColor;
};

/**
 * Registry-Aware Widget (The NEW Way)
 * Usage: layout->addWidget(new SensorWidget<BoilerPressureTag>(registry));
 */
template<typename T = void>
class SensorWidget : public SensorWidgetBase {
public:
    SensorWidget(ISensorRegistry* registry = nullptr) : _registry(registry) {}

    void setRegistry(ISensorRegistry* registry) override {
        _registry = registry;
    }

    void refresh() override {
        if (_registry) {
            update(_registry->getLatest<T>());
        }
    }

private:
    ISensorRegistry* _registry;
};

/**
 * Direct-Sensor Widget (The LEGACY Way)
 * Specialization for 'void' allows co-existence during migration.
 * Usage: layout->addWidget(new SensorWidget(sensor_ptr));
 */
template<>
class SensorWidget<void> : public SensorWidgetBase {
public:
    SensorWidget(ISensor* sensor) : _sensor(sensor) {}

    void refresh() override {
        if (_sensor) {
            update(_sensor->getReading());
        }
    }

private:
    ISensor* _sensor;
};

#endif
