#ifndef SCREEN_LAYOUT_H
#define SCREEN_LAYOUT_H

#include <lvgl.h>
#include <map>
#include <vector>
#include "IWidget.h"
#include "../Themes/ITheme.h"

class ScreenLayout {
public:
    ScreenLayout();
    
    void init(lv_obj_t* parent, uint8_t cols = 2, uint8_t rows = 2);
    void update();
    void applyTheme(ITheme* theme);
    void reset();
    
    void addWidget(IWidget* widget);
    void showMessage(const String& text);

private:
    lv_obj_t* _grid;
    std::vector<IWidget*> _widgets;
    ITheme* _currentTheme;
    uint8_t _cols;
    uint8_t _rows;
    int _nextSlot;
};

#endif
