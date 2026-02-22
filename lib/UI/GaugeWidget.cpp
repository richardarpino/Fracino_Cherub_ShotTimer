#include "GaugeWidget.h"

GaugeWidget::GaugeWidget(ISensor* sensor) 
    : _container(nullptr), _meter(nullptr), _scale(nullptr), _indic(nullptr), _sensor(sensor) {}

lv_obj_t* GaugeWidget::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    // Parent Container (Full Screen)
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_opa(_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    // Meter (Centered in Container)
    _meter = lv_meter_create(_container);
    lv_obj_set_size(_meter, 130, 130); 
    lv_obj_center(_meter);
    
    // Transparent background and no border for meter
    lv_obj_set_style_bg_opa(_meter, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(_meter, 0, 0);
    lv_obj_set_style_radius(_meter, 0, 0); 
    lv_obj_set_style_outline_width(_meter, 0, 0);

    // Create a scale
    _scale = lv_meter_add_scale(_meter);
    lv_meter_set_scale_ticks(_meter, _scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(_meter, _scale, 2, 4, 15, lv_color_black(), 10);
    
    // 270 degrees, starting at 135 (bottom left)
    lv_meter_set_scale_range(_meter, _scale, 0, 100, 270, 135);

    // Add a needle line indicator
    _indic = lv_meter_add_needle_line(_meter, _scale, 3, lv_palette_main(LV_PALETTE_GREY), -10);

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
        lv_meter_set_scale_range(_meter, _scale, meta.low.value, meta.high.value, 270, 135);
    }

    lv_meter_set_indicator_value(_meter, _indic, reading.value);

    // Dynamic Coloring based on Alert State
    if (reading.isError) {
        // Alert Mode
        lv_obj_set_style_bg_color(_container, _alertBgColor, 0);
        lv_obj_set_style_bg_opa(_container, LV_OPA_COVER, 0);
        
        // Numbers (labels) -> Alert Text Color
        lv_obj_set_style_text_color(_meter, _errorColor, LV_PART_MAIN);
        
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
}
