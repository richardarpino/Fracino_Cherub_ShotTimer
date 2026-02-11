#ifndef SCREEN_LAYOUT_H
#define SCREEN_LAYOUT_H

#include <lvgl.h>
#include <map>
#include <vector>
#include "IWidget.h"

class ScreenLayout {
public:
    ScreenLayout();
    
    void init(lv_obj_t* parent);
    void update();
    void applyTheme(ITheme* theme);
    
    void addWidget(IWidget* widget);
    void showMessage(const String& text);

private:
    lv_obj_t* _grid;
    std::vector<IWidget*> _widgets;
    int _nextSlot;
};

#endif
