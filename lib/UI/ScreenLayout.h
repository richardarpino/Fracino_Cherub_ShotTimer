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
    
    void setDimensions(uint8_t cols, uint8_t rows);
    void init(lv_obj_t* parent);
    void update();
    void applyTheme(ITheme* theme);
    void reset();
    
    void addWidget(IWidget* widget);
    void showMessage(const char* text);
    void setRegistry(class ISensorRegistry* registry);
    lv_obj_t* getGrid() { return _grid; }

private:
    void initializeWidget(IWidget* widget, int slot);
    
    lv_obj_t* _grid;
    std::vector<IWidget*> _widgets;
    class ISensorRegistry* _registry;
    ITheme* _currentTheme;
    uint8_t _cols;
    uint8_t _rows;
    int _nextSlot;
};

#endif
