#include "ScreenLayout.h"
#include "SensorWidget.h"
#include "StatusWidget.h"

ScreenLayout::ScreenLayout() 
    : _grid(nullptr), _widgets(), _initialized(false), _parent(nullptr), _registry(nullptr), _currentTheme(nullptr), _cols(2), _rows(2), _nextSlot(0) {
    memset(_col_dsc, 0, sizeof(_col_dsc));
    memset(_row_dsc, 0, sizeof(_row_dsc));
}

void ScreenLayout::setDimensions(uint8_t cols, uint8_t rows) {
    _cols = cols;
    _rows = rows;
}

void ScreenLayout::init(lv_obj_t* parent) {
    if (!parent) return;

    // Force screen background to black to prevent white lines/gaps
    lv_obj_set_style_bg_color(parent, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, 0);

    // Reset initialization state
    _initialized = false;
    _parent = parent;
    
    // Always create a new grid to avoid dangling pointers after lv_obj_clean(parent)
    _grid = lv_obj_create(parent);
    if (!_grid) return;
    
    lv_obj_set_size(_grid, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(_grid, 0, 0);
    lv_obj_set_style_pad_row(_grid, 0, 0);
    lv_obj_set_style_pad_column(_grid, 0, 0);
    lv_obj_set_style_border_width(_grid, 0, 0);
    lv_obj_set_style_bg_opa(_grid, LV_OPA_TRANSP, 0);

    for(int i=0; i<_cols && i<9; i++) _col_dsc[i] = LV_GRID_FR(1);
    _col_dsc[_cols < 9 ? _cols : 9] = LV_GRID_TEMPLATE_LAST;

    for(int i=0; i<_rows && i<9; i++) _row_dsc[i] = LV_GRID_FR(1);
    _row_dsc[_rows < 9 ? _rows : 9] = LV_GRID_TEMPLATE_LAST;

    lv_obj_set_grid_dsc_array(_grid, _col_dsc, _row_dsc);
    lv_obj_set_layout(_grid, LV_LAYOUT_GRID);
    
    for (int i = 0; i < _widgets.size(); i++) {
        initializeWidget(_widgets[i], i);
    }
    _initialized = true;
}

void ScreenLayout::reset() {
    _nextSlot = 0;
    _initialized = false;
    for (auto* w : _widgets) {
        delete w;
    }
    _widgets.clear();

    if (_grid) {
        lv_obj_del(_grid);
        _grid = nullptr;
    }
}

void ScreenLayout::addWidget(IWidget* widget) {
    if (!widget) return;
    if (_nextSlot >= (_cols * _rows)) return; 

    _widgets.push_back(widget);
    
    if (_grid) {
        initializeWidget(widget, _nextSlot);
    }
    
    _nextSlot++;
}

void ScreenLayout::initializeWidget(IWidget* widget, int slot) {
    if (!_grid || !widget) return;

    uint8_t col = slot / _rows;
    uint8_t row = slot % _rows;

    lv_obj_t* root = widget->init(_grid, _cols, _rows);
    
    if (root) {
        lv_obj_set_grid_cell(root, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
    }

    if (_currentTheme) widget->applyTheme(_currentTheme);
    if (_registry) widget->setRegistry(_registry);
}

void ScreenLayout::update() {
    for (auto* w : _widgets) {
        if (w) w->refresh();
    }
}

void ScreenLayout::showMessage(const char* text) {
    StatusMessage m("Notification", text, -1.0f, false);
    for (auto* w : _widgets) {
        if (w && w->isStatusWidget()) {
            w->update(m);
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
        lv_obj_set_style_bg_opa(_grid, LV_OPA_COVER, 0);
    }
}

void ScreenLayout::setRegistry(ISensorRegistry* registry) {
    _registry = registry;
    for (auto* w : _widgets) {
        w->setRegistry(registry);
    }
}
