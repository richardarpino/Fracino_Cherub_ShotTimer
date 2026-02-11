#ifndef STATUS_WIDGET_H
#define STATUS_WIDGET_H

#include <lvgl.h>
#include <Arduino.h>
#include "../Themes/ITheme.h"
#include "IWidget.h"

class StatusWidget : public IWidget {
public:
    StatusWidget(ISensor* sensor = nullptr);
    
    lv_obj_t* init(lv_obj_t* parent) override;
    void update(const Reading& reading) override;
    void refresh() override;
    void applyTheme(ITheme* theme) override;
    
    bool isStatusWidget() override { return true; }
    
    // Explicit text setting (manual use)
    void setText(const char* text);

private:
    lv_obj_t* _container;
    lv_obj_t* _label;
    ISensor* _sensor;
    unsigned long _messageTimeout;
};

#endif
