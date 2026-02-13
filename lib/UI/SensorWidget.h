#ifndef SENSOR_WIDGET_H
#define SENSOR_WIDGET_H

#include <lvgl.h>
#include "IWidget.h"

class SensorWidget : public IWidget {
public:
    SensorWidget(ISensor* sensor = nullptr, bool largeFont = false);
    
    lv_obj_t* init(lv_obj_t* parent) override;
    void update(const Reading& reading) override;
    void refresh() override;
    void applyTheme(ITheme* theme) override;

private:
    lv_obj_t* _container;
    lv_obj_t* _value_label;
    lv_obj_t* _unit_label;
    
    ISensor* _sensor;
    bool _largeFont;
    
    // Store theme colors for error state reversal
    lv_color_t _textColor;
    lv_color_t _labelColor;
};

#endif
