#ifndef GAUGE_WIDGET_H
#define GAUGE_WIDGET_H

#include <lvgl.h>
#include "IWidget.h"

class GaugeWidget : public IWidget {
public:
    GaugeWidget(ISensor* sensor = nullptr);
    
    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override;
    void update(const Reading& reading) override;
    void refresh() override;
    void applyTheme(ITheme* theme) override;

private:
    lv_obj_t* _container;
    lv_obj_t* _meter;
    lv_meter_scale_t* _scale;
    lv_meter_indicator_t* _indic;
    ISensor* _sensor;
    
    lv_color_t _bgColor;
    lv_color_t _textColor;
    lv_color_t _errorColor;
    lv_color_t _labelColor;
    lv_color_t _alertBgColor;
};

#endif
