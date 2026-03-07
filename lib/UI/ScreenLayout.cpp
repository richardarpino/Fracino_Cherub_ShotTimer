#include "ScreenLayout.h"
#include "SensorWidget.h"
#include "StatusWidget.h"

ScreenLayout::ScreenLayout() 
    : _grid(nullptr), _widgets(), _registry(nullptr), _currentTheme(nullptr), _cols(2), _rows(2), _nextSlot(0) {}

void ScreenLayout::setDimensions(uint8_t cols, uint8_t rows) {
    _cols = cols;
    _rows = rows;
}

void ScreenLayout::init(lv_obj_t* parent) {
    _grid = parent;
    
    if (!_grid) return;

    // Use internal dimensions
    uint8_t cols = _cols;
    uint8_t rows = _rows;
    _nextSlot = 0;

    // Use persistable members to prevent memory corruption in LVGL grid
    static lv_coord_t col_dsc[10];
    static lv_coord_t row_dsc[10];

    for(int i=0; i<cols && i<9; i++) col_dsc[i] = LV_GRID_FR(1);
    col_dsc[cols < 9 ? cols : 9] = LV_GRID_TEMPLATE_LAST;

    for(int i=0; i<rows && i<9; i++) row_dsc[i] = LV_GRID_FR(1);
    row_dsc[rows < 9 ? rows : 9] = LV_GRID_TEMPLATE_LAST;

    lv_obj_set_grid_dsc_array(_grid, col_dsc, row_dsc);
    lv_obj_set_layout(_grid, LV_LAYOUT_GRID);

    // Initialize all deferred widgets
    for (int i = 0; i < _widgets.size(); i++) {
        initializeWidget(_widgets[i], i);
    }
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

    _widgets.push_back(widget);
    
    if (_grid) {
        initializeWidget(widget, _nextSlot);
    }
    
    _nextSlot++;
}

void ScreenLayout::initializeWidget(IWidget* widget, int slot) {
    if (!_grid) return;

    // Determine grid position (down then right)
    uint8_t col = slot / _rows;
    uint8_t row = slot % _rows;

    // Initialize the widget and get its root visual
    lv_obj_t* root = widget->init(_grid, _cols, _rows);
    
    // Layout-specific placement logic stays here in the manager
    lv_obj_set_grid_cell(root, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);

    if (_currentTheme) widget->applyTheme(_currentTheme);
    if (_registry) widget->setRegistry(_registry);
}

void ScreenLayout::update() {
    for (auto* w : _widgets) {
        w->refresh();
    }
}

void ScreenLayout::showMessage(const char* text) {
    for (auto* w : _widgets) {
        if (w->isStatusWidget()) {
            StatusWidgetBase* sw = static_cast<StatusWidgetBase*>(w);
            StatusMessage m("Notification", text, -1.0f, false);
            sw->update(m);
            return; // Found the status area
        }
    }
}

void ScreenLayout::applyTheme(ITheme* theme) {
    if (!theme) return;
    _currentTheme = theme;
    for (auto* w : _widgets) {
        w->applyTheme(theme);
    }
    
    // Grid background
    uint16_t c = theme->getBackgroundColor();
    uint8_t r = (c >> 11) & 0x1F; r = (r * 255) / 31;
    uint8_t g = (c >> 5) & 0x3F;  g = (g * 255) / 63;
    uint8_t b = c & 0x1F;         b = (b * 255) / 31;
    
    if (_grid) {
        lv_obj_set_style_bg_color(_grid, lv_color_make(r, g, b), 0);
    }
}

void ScreenLayout::setRegistry(ISensorRegistry* registry) {
    _registry = registry;
    for (auto* w : _widgets) {
        w->setRegistry(registry);
    }
}
