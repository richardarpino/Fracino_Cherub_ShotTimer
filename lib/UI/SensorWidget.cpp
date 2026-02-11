#include "SensorWidget.h"
#include <stdio.h>

SensorWidget::SensorWidget(ISensor* sensor, bool largeFont) 
    : _container(nullptr), _value_label(nullptr), _unit_label(nullptr), _sensor(sensor), _largeFont(largeFont) {}

lv_obj_t* SensorWidget::init(lv_obj_t* parent) {
    // Container
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(_container, 2, 0); // Tighter padding
    lv_obj_set_style_pad_bottom(_container, 8, 0); // Extra room for unit
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_flex_flow(_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(_container, 0, 0); // No gap between value and unit
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Value (Middle)
    _value_label = lv_label_create(_container);
    lv_label_set_text(_value_label, "---");
    const lv_font_t* font = _largeFont ? &lv_font_montserrat_32 : &lv_font_montserrat_28;
    lv_obj_set_style_text_font(_value_label, font, 0);

    // Unit (Bottom)
    _unit_label = lv_label_create(_container);
    lv_label_set_text(_unit_label, "");
    lv_obj_set_style_text_font(_unit_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_translate_y(_unit_label, -4, 0); // Pull unit closer to large numbers
    return _container;
}

void SensorWidget::refresh() {
    if (_sensor) {
        update(_sensor->getReading());
    }
}

void SensorWidget::update(const Reading& reading) {
    if (!reading.isValid) return;

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

    lv_color_t bg = toLvColor(theme->getBackgroundColor());
    lv_color_t text = toLvColor(theme->getTextColor());
    lv_color_t label = toLvColor(theme->getLabelColor());

    // Apply
    lv_obj_set_style_bg_color(_container, bg, 0);
    lv_obj_set_style_text_color(_value_label, text, 0);
    lv_obj_set_style_text_color(_unit_label, label, 0);
}

