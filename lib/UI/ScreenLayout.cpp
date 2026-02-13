#include "ScreenLayout.h"
#include "SensorWidget.h"
#include "StatusWidget.h"

ScreenLayout::ScreenLayout() : _grid(nullptr), _nextSlot(0) {}

void ScreenLayout::init(lv_obj_t* parent) {
    _grid = parent;

    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(_grid, col_dsc, row_dsc);
    lv_obj_set_layout(_grid, LV_LAYOUT_GRID);
}

void ScreenLayout::addWidget(IWidget* widget) {
    if (_nextSlot >= 4) return; // Grid full

    // Determine grid position (down then right)
    uint8_t col = _nextSlot / 2;
    uint8_t row = _nextSlot % 2;

    // Initialize the widget and get its root visual
    lv_obj_t* root = widget->init(_grid);
    
    // Layout-specific placement logic stays here in the manager
    lv_obj_set_grid_cell(root, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

    _widgets.push_back(widget);
    _nextSlot++;
}

void ScreenLayout::update() {
    for (auto* w : _widgets) {
        w->refresh();
    }
}

void ScreenLayout::showMessage(const String& text) {
    for (auto* w : _widgets) {
        if (w->isStatusWidget()) {
            StatusWidget* sw = static_cast<StatusWidget*>(w);
            Reading r(0, "", text, 0, false);
            sw->update(r);
            return; // Found the status area
        }
    }
}

void ScreenLayout::applyTheme(ITheme* theme) {
    for (auto* w : _widgets) {
        w->applyTheme(theme);
    }
    
    // Grid background
    uint16_t c = theme->getBackgroundColor();
    uint8_t r = (c >> 11) & 0x1F; r = (r * 255) / 31;
    uint8_t g = (c >> 5) & 0x3F;  g = (g * 255) / 63;
    uint8_t b = c & 0x1F;         b = (b * 255) / 31;
    
    lv_obj_set_style_bg_color(_grid, lv_color_make(r, g, b), 0);
}
