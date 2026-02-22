#include "StatusWidget.h"
#include <Arduino.h>

StatusWidget::StatusWidget(ISensor* sensor) 
    : _container(nullptr), _label(nullptr), _sensor(sensor), _messageTimeout(0) {}

lv_obj_t* StatusWidget::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(_container, 2, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_flex_flow(_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    _label = lv_label_create(_container);
    lv_label_set_text(_label, "READY");
    lv_label_set_long_mode(_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(_label, LV_TEXT_ALIGN_CENTER, 0);
    
    // Auto-detect font size based on grid columns
    const lv_font_t* font = (cols == 1) ? &lv_font_montserrat_20 : &lv_font_montserrat_12;
    lv_obj_set_style_text_font(_label, font, 0);
    
    return _container;
}

void StatusWidget::setText(const char* text) {
    if (_label) lv_label_set_text(_label, text);
}

void StatusWidget::refresh() {
    // Only pull from sensor if no manual message is currently "locked"
    if (_sensor && millis() > _messageTimeout) {
        Reading r = _sensor->getReading();
        setText(r.label.c_str());
        
        // Semantic Styling: Turn red if in error state
        if (r.isError) {
            lv_obj_set_style_text_color(_label, _errorColor, 0);
        } else {
            lv_obj_set_style_text_color(_label, _textColor, 0);
        }
    }
}

void StatusWidget::update(const Reading& reading) {
    // No more update guards. Seniors choose what to display.
    
    // Manual push (usually from ScreenLayout::showMessage or showInfo)
    if (reading.label.length() > 0) {
        setText(reading.label.c_str());
        
        // Also handle styling for manual pushes (usually not errors, but let's be consistent)
        if (reading.isError) {
            lv_obj_set_style_text_color(_label, _errorColor, 0);
        } else {
            lv_obj_set_style_text_color(_label, _textColor, 0);
        }

        // Lock the display for 3 seconds to prevent immediate overwrite by refresh()
        _messageTimeout = millis() + 3000;
    }
}

void StatusWidget::applyTheme(ITheme* theme) {
    // Helper lambda (duplicate, but simple enough)
    auto toLvColor = [](uint16_t c) -> lv_color_t {
        uint8_t r = (c >> 11) & 0x1F; r = (r * 255) / 31;
        uint8_t g = (c >> 5) & 0x3F;  g = (g * 255) / 63;
        uint8_t b = c & 0x1F;         b = (b * 255) / 31;
        return lv_color_make(r, g, b);
    };

    lv_color_t bg = toLvColor(theme->getBackgroundColor());
    _textColor = toLvColor(theme->getLabelColor()); // Use label color for status text
    _errorColor = toLvColor(theme->getAlertTextColor());

    lv_obj_set_style_bg_color(_container, bg, 0);
    lv_obj_set_style_text_color(_label, _textColor, 0);
}
