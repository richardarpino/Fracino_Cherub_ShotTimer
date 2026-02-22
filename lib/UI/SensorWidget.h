#ifndef SENSOR_WIDGET_H
#define SENSOR_WIDGET_H

#include <lvgl.h>
#include "IWidget.h"

class SensorWidget : public IWidget {
public:
    SensorWidget(ISensor* sensor = nullptr);
    
    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override;
    void update(const Reading& reading) override;
    void refresh() override;
    void applyTheme(ITheme* theme) override;

private:
    lv_obj_t* _container;
    lv_obj_t* _value_label;
    lv_obj_t* _unit_label;
    
    ISensor* _sensor;
    
    // Store theme colors for error state reversal
    lv_color_t _textColor;
    lv_color_t _labelColor;
    lv_color_t _errorColor;
};

#endif
