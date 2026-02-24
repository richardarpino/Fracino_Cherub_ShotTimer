#include "ScreenLayout.h"
#include "SensorWidget.h"
#include "StatusWidget.h"

ScreenLayout::ScreenLayout() : _grid(nullptr), _widgets(), _currentTheme(nullptr), _nextSlot(0) {}

void ScreenLayout::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    _grid = parent;
    _cols = cols;
    _rows = rows;
    _nextSlot = 0;

    // Use dynamic descriptors for the grid
    lv_coord_t* col_dsc = (lv_coord_t*)malloc((cols + 1) * sizeof(lv_coord_t));
    lv_coord_t* row_dsc = (lv_coord_t*)malloc((rows + 1) * sizeof(lv_coord_t));

    for(int i=0; i<cols; i++) col_dsc[i] = LV_GRID_FR(1);
    col_dsc[cols] = LV_GRID_TEMPLATE_LAST;

    for(int i=0; i<rows; i++) row_dsc[i] = LV_GRID_FR(1);
    row_dsc[rows] = LV_GRID_TEMPLATE_LAST;

    lv_obj_set_grid_dsc_array(_grid, col_dsc, row_dsc);
    lv_obj_set_layout(_grid, LV_LAYOUT_GRID);
    
    // Note: LVGL copies these arrays, but with set_grid_dsc_array it doesn't. 
    // Wait, LVGL 8.3 requires these to be static or persist. 
    // Let's use static for common cases if possible, but for dynamic we need a better way.
    // Actually, ScreenLayout is usually long-lived. We can store them as members.
}

void ScreenLayout::reset() {
    _nextSlot = 0;
    // Delete all widgets to free memory
    for (auto* w : _widgets) {
        delete w;
    }
    _widgets.clear();

    // LVGL: Delete children of the grid
    if (_grid) {
        lv_obj_clean(_grid);
    }
}

void ScreenLayout::addWidget(IWidget* widget) {
    if (_nextSlot >= (_cols * _rows)) return; // Grid full

    // Determine grid position (down then right)
    uint8_t col = _nextSlot / _rows;
    uint8_t row = _nextSlot % _rows;

    // Initialize the widget and get its root visual
    lv_obj_t* root = widget->init(_grid, _cols, _rows);
    
    // Layout-specific placement logic stays here in the manager
    lv_obj_set_grid_cell(root, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

    if (_currentTheme) widget->applyTheme(_currentTheme);

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
    _currentTheme = theme;
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
