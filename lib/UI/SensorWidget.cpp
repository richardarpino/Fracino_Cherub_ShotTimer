#include "SensorWidget.h"
#include <stdio.h>

extern "C" {
LV_FONT_DECLARE(lv_font_roboto_mono_32)
LV_FONT_DECLARE(lv_font_roboto_mono_28)
}

SensorWidget::SensorWidget(ISensor* sensor) 
    : _container(nullptr), _value_label(nullptr), _unit_label(nullptr), _sensor(sensor) {}

lv_obj_t* SensorWidget::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    // Container
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(_container, 4, 0); // Uniform padding
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_flex_flow(_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(_container, 2, 0); // Small gap between value and unit
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Value (Middle)
    _value_label = lv_label_create(_container);
    lv_label_set_text(_value_label, "---");
    const lv_font_t* font = (cols == 1) ? &lv_font_roboto_mono_32 : &lv_font_roboto_mono_28;
    lv_obj_set_style_text_font(_value_label, font, 0);

    // Unit (Bottom)
    _unit_label = lv_label_create(_container);
    lv_label_set_text(_unit_label, "");
    lv_obj_set_style_text_font(_unit_label, &lv_font_montserrat_12, 0);
    return _container;
}

void SensorWidget::refresh() {
    if (_sensor) {
        update(_sensor->getReading());
    }
}

void SensorWidget::update(const Reading& reading) {
    // We no longer block updates! Sensors decide what to show (e.g. Last Known Good).

    // Update Value with precision from reading
    char buf[16];
    if (reading.precision == 0) {
        snprintf(buf, sizeof(buf), "%.0f", reading.value);
    } else {
        snprintf(buf, sizeof(buf), "%.*f", reading.precision, reading.value);
    }
    lv_label_set_text(_value_label, buf);

    // Update Unit from reading
    lv_label_set_text(_unit_label, reading.unit.c_str());

    // Semantic Error Styling: Change color if in error state
    if (reading.isError) {
        lv_obj_set_style_bg_color(_container, _alertBgColor, 0);
        lv_obj_set_style_text_color(_value_label, _errorColor, 0);
        lv_obj_set_style_text_color(_unit_label, _errorColor, 0);
    } else {
        lv_obj_set_style_bg_color(_container, _bgColor, 0);
        lv_obj_set_style_text_color(_value_label, _textColor, 0);
        lv_obj_set_style_text_color(_unit_label, _labelColor, 0);
    }
}

void SensorWidget::applyTheme(ITheme* theme) {
    // Helper lambda to convert 565 to LVGL color
    auto toLvColor = [](uint16_t c) -> lv_color_t {
        uint8_t r = (c >> 11) & 0x1F;
        uint8_t g = (c >> 5) & 0x3F;
        uint8_t b = c & 0x1F;
        // Scale to 8-bit
        r = (r * 255) / 31;
        g = (g * 255) / 63;
        b = (b * 255) / 31;
        return lv_color_make(r, g, b);
    };

    _textColor = toLvColor(theme->getTextColor());
    _labelColor = toLvColor(theme->getLabelColor());
    _errorColor = toLvColor(theme->getAlertTextColor()); 
    _bgColor = toLvColor(theme->getBackgroundColor());
    _alertBgColor = toLvColor(theme->getAlertBackgroundColor());

    // Apply
    lv_obj_set_style_bg_color(_container, _bgColor, 0);
    lv_obj_set_style_text_color(_value_label, _textColor, 0);
    lv_obj_set_style_text_color(_unit_label, _labelColor, 0);
}
