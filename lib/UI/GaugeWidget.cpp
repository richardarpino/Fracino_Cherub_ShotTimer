#include "GaugeWidget.h"

GaugeWidget::GaugeWidget(ISensor* sensor) 
    : _container(nullptr), _meter(nullptr), _scale(nullptr), _indic(nullptr), _unit_label(nullptr), _sensor(sensor) {}

lv_obj_t* GaugeWidget::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    // Ensure parent layout is updated so we can read its size
    lv_obj_update_layout(parent);
    lv_coord_t parent_w = lv_obj_get_width(parent);
    lv_coord_t parent_h = lv_obj_get_height(parent);
    
    // Fallback to screen dimensions (Landscape: 240x135) if parent layout not yet ready
    if (parent_w <= 0) parent_w = 240; 
    if (parent_h <= 0) parent_h = 135;

    // Calculate target cell size
    lv_coord_t cell_w = parent_w / cols;
    lv_coord_t cell_h = parent_h / rows;
    
    // Maximize diameter - fill cell almost completely
    // For 2x2, we "oversize" relative to the cell height to maximize legibility, 
    // but we clamp it to both width and height to prevent clipping.
    lv_coord_t diameter;
    if (cols == 2 && rows == 2) {
        diameter = (cell_h * 11) / 10;
        if (diameter > cell_w - 2) diameter = cell_w - 2;
        if (diameter > cell_h - 2) diameter = cell_h - 2;
    } else {
        diameter = (cell_w < cell_h ? cell_w : cell_h) - 4;
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
    
    _tick_count = (isFull || isHalf) ? 11 : 7;
    _major_tick_len = isFull ? 14 : (isHalf ? 10 : 8);
    _tick_len = isFull ? 10 : (isHalf ? 8 : 6);
    _label_gap = isFull ? 18 : (isHalf ? 12 : 4);
    
    uint16_t needle_width = isFull ? 8 : (isHalf ? 6 : 4);
    const lv_font_t* dial_font = (isFull || isHalf) ? &lv_font_montserrat_14 : &lv_font_montserrat_12;
    const lv_font_t* unit_font = isFull ? &lv_font_montserrat_18 : (isHalf ? &lv_font_montserrat_14 : &lv_font_montserrat_12);

    lv_meter_set_scale_ticks(_meter, _scale, _tick_count, 2, _tick_len, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(_meter, _scale, 2, 4, _major_tick_len, lv_color_black(), _label_gap);
    
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
        
        // Dial (ticks) -> Error Color
        lv_meter_set_scale_ticks(_meter, _scale, _tick_count, 2, _tick_len, _errorColor);
        lv_meter_set_scale_major_ticks(_meter, _scale, 2, 4, _major_tick_len, _errorColor, _label_gap);
    } else {
        // Normal Mode
        lv_obj_set_style_bg_color(_container, _bgColor, 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
        
        // Dial (ticks)
        lv_meter_set_scale_ticks(_meter, _scale, _tick_count, 2, _tick_len, _textColor);
        lv_meter_set_scale_major_ticks(_meter, _scale, 2, 4, _major_tick_len, _labelColor, _label_gap);
        
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
    if(dsc->part == LV_PART_TICKS && dsc->type == LV_METER_DRAW_PART_TICK) {
        if(dsc->label_dsc != NULL) {
            float val = (float)dsc->value / 10.0f;
            
            // Custom formatting: drop leading 0 for decimals < 1 (e.g. .5)
            if (val > 0.0f && val < 1.0f) {
                lv_snprintf(dsc->text, 16, ".%d", (int)(val * 10));
            } else if (val == (int)val) {
                lv_snprintf(dsc->text, 16, "%d", (int)val);
            } else {
                int whole = (int)val;
                int frac = (int)((val - whole) * 10 + 0.5f);
                if (frac >= 10) { whole++; frac = 0; }
                lv_snprintf(dsc->text, 16, "%d.%d", whole, frac);
            }
        }
    }
}
