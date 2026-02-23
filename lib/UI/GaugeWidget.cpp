#include "GaugeWidget.h"

GaugeWidget::GaugeWidget(ISensor* sensor) 
    : _container(nullptr), _meter(nullptr), _scale(nullptr), _indic(nullptr), _unit_label(nullptr), _sensor(sensor) {}

lv_obj_t* GaugeWidget::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    // Ensure parent layout is updated so we can read its size
    lv_obj_update_layout(parent);
    lv_coord_t w = lv_obj_get_width(parent);
    lv_coord_t h = lv_obj_get_height(parent);
    
    // Maximize diameter - fill cell almost completely (2px padding)
    // For 2x2, we oversize by 10% as requested to maximize legibility
    lv_coord_t diameter;
    if (cols == 2 && rows == 2) {
        diameter = (h * 11) / 10;
    } else {
        diameter = (w < h ? w : h) - 2;
    }
    
    if (diameter < 40) diameter = 40; 
    
    // Parent Container (Full Screen)
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Meter (Centered in Container)
    _meter = lv_meter_create(_container);
    lv_obj_set_size(_meter, diameter, diameter); 
    lv_obj_center(_meter);
    
    // Transparent background and no border for meter
    lv_obj_set_style_bg_opa(_meter, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_meter, 0, 0);
    lv_obj_set_style_radius(_meter, 0, 0); 
    lv_obj_set_style_outline_width(_meter, 0, 0);

    // Create a scale
    _scale = lv_meter_add_scale(_meter);
    
    // Scaling Logic based on Grid Layout (PREDICTABLE)
    bool isFull = (cols == 1 && rows == 1);
    bool isHalf = (rows == 1 && cols == 2) || (rows == 2 && cols == 1);
    
    uint16_t tick_count = (isFull || isHalf) ? 11 : 7;
    lv_coord_t major_tick_len = isFull ? 10 : (isHalf ? 6 : 5);
    lv_coord_t tick_len = isFull ? 6 : (isHalf ? 4 : 2);
    lv_coord_t label_gap = isFull ? 18 : (isHalf ? 12 : 2);
    
    uint16_t needle_width = isFull ? 8 : (isHalf ? 6 : 4);
    const lv_font_t* dial_font = (isFull || isHalf) ? &lv_font_montserrat_14 : &lv_font_montserrat_12;
    const lv_font_t* unit_font = isFull ? &lv_font_montserrat_18 : (isHalf ? &lv_font_montserrat_14 : &lv_font_montserrat_12);

    lv_meter_set_scale_ticks(_meter, _scale, tick_count, 2, tick_len, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(_meter, _scale, 2, 4, major_tick_len, lv_color_black(), label_gap);
    
    // Scale Dial Labels (Numbers)
    lv_obj_set_style_text_font(_meter, dial_font, 0);
    if (!isFull && !isHalf) {
        // Hide dial labels for 2x2 as they are too small/cluttered
        lv_obj_set_style_text_opa(_meter, LV_OPA_TRANSP, 0);
    }
    
    // 270 degrees, starting at 135 (bottom left)
    // Scale by 10x internally to support decimal increments without truncation
    lv_meter_set_scale_range(_meter, _scale, 0, 1000, 270, 135);
    
    // Add custom event to format labels as decimals
    lv_obj_add_event_cb(_meter, meter_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    // Bolder markers and needle
    _indic = lv_meter_add_needle_line(_meter, _scale, needle_width, lv_palette_main(LV_PALETTE_GREY), -1);

    // Unit Label (Centered under pivot)
    // Create as child of _container (not _meter) to avoid inheriting scale label transparency
    _unit_label = lv_label_create(_container);
    lv_label_set_text(_unit_label, "");
    lv_obj_set_style_text_font(_unit_label, unit_font, 0);
    
    // Vertical positioning under pivot (centered in container, then nudged)
    lv_coord_t y_offset = (isFull || isHalf) ? (diameter / 3) + 5 : (diameter / 4) + 5;
    lv_obj_align(_unit_label, LV_ALIGN_CENTER, 0, y_offset);

    return _container;
}

void GaugeWidget::refresh() {
    if (_sensor) {
        update(_sensor->getReading());
    }
}

void GaugeWidget::update(const Reading& reading) {
    if (!_meter || !_indic) return;

    // Use metadata to scale the gauge if possible
    if (_sensor) {
        SensorMetadata meta = _sensor->getMetadata();
        // Scale range by 10x for precision (prevent integer truncation)
        lv_meter_set_scale_range(_meter, _scale, (int32_t)(meta.low.value * 10), (int32_t)(meta.high.value * 10), 270, 135);
    }

    // Scale reading by 10x
    lv_meter_set_indicator_value(_meter, _indic, (int32_t)(reading.value * 10));

    // Update Unit Label
    if (_unit_label) {
        lv_label_set_text(_unit_label, reading.unit.c_str());
    }

    // Dynamic Coloring based on Alert State
    if (reading.isError) {
        // Alert Mode
        lv_obj_set_style_bg_color(_container, _alertBgColor, 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
        
        // Numbers (labels) -> Alert Text Color
        lv_obj_set_style_text_color(_meter, _errorColor, LV_PART_MAIN);
        if (_unit_label) lv_obj_set_style_text_color(_unit_label, _errorColor, 0);
        
        // Ensure needle also visible/semantically correct in alert
        lv_obj_set_style_line_color(_meter, _errorColor, LV_PART_INDICATOR);
    } else {
        // Normal Mode
        lv_obj_set_style_bg_color(_container, _bgColor, 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
        
        // Dial (ticks) -> Text Color
        lv_meter_set_scale_ticks(_meter, _scale, 11, 2, 10, _textColor);
        
        // Numbers (labels) -> Label Color
        lv_obj_set_style_text_color(_meter, _labelColor, LV_PART_MAIN);
        if (_unit_label) lv_obj_set_style_text_color(_unit_label, _labelColor, 0);
        
        // Pointer (needle) -> Label Color
        lv_obj_set_style_line_color(_meter, _labelColor, LV_PART_INDICATOR);
    }
}

void GaugeWidget::applyTheme(ITheme* theme) {
    auto toLvColor = [](uint16_t c) -> lv_color_t {
        uint8_t r = (c >> 11) & 0x1F; r = (r * 255) / 31;
        uint8_t g = (c >> 5) & 0x3F;  g = (g * 255) / 63;
        uint8_t b = c & 0x1F;         b = (b * 255) / 31;
        return lv_color_make(r, g, b);
    };

    _bgColor = toLvColor(theme->getBackgroundColor());
    _textColor = toLvColor(theme->getTextColor());
    _labelColor = toLvColor(theme->getLabelColor());
    _errorColor = toLvColor(theme->getAlertTextColor());
    _alertBgColor = toLvColor(theme->getAlertBackgroundColor());

    // Initial styling (assumed normal until first update)
    lv_obj_set_style_bg_color(_container, _bgColor, 0);
    lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
    if (_unit_label) lv_obj_set_style_text_color(_unit_label, _labelColor, 0);
}

void GaugeWidget::meter_event_cb(lv_event_t* e) {
    lv_obj_draw_part_dsc_t* dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);
    if(dsc->part == LV_PART_TICKS && dsc->id == LV_METER_DRAW_PART_TICK) {
        if(dsc->label_dsc != NULL) {
            float val = (float)dsc->value / 10.0f;
            
            // Custom formatting: drop leading 0 for decimals < 1 (e.g. .5)
            if (val > 0.0f && val < 1.0f) {
                lv_snprintf(dsc->text, 16, ".%d", (int)(val * 10));
            } else if (val == (int)val) {
                lv_snprintf(dsc->text, 16, "%d", (int)val);
            } else {
                lv_snprintf(dsc->text, 16, "%.1f", val);
            }
        }
    }
}
