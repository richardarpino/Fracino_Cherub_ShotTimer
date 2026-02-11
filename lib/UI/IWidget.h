#ifndef IWIDGET_H
#define IWIDGET_H

#include <lvgl.h>
#include "../Interfaces/ISensor.h"
#include "../Themes/ITheme.h"

class IWidget {
public:
    virtual ~IWidget() {}
    virtual lv_obj_t* init(lv_obj_t* parent) = 0;
    virtual void update(const Reading& reading) = 0;
    virtual void refresh() = 0;
    virtual void applyTheme(ITheme* theme) = 0;
    
    // RTTI-free type checking
    virtual bool isStatusWidget() { return false; }
};

#endif
