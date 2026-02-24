#include "BlockerWidget.h"

BlockerWidget::BlockerWidget(IBlocker* blocker) 
    : _container(nullptr), _status_label(nullptr), _bar(nullptr), _blocker(blocker) {}

lv_obj_t* BlockerWidget::init(lv_obj_t* parent, uint8_t cols, uint8_t rows) {
    _container = lv_obj_create(parent);
    lv_obj_set_size(_container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(_container, 10, 0);
    lv_obj_set_style_border_width(_container, 0, 0);
    lv_obj_set_style_radius(_container, 0, 0);
    lv_obj_set_flex_flow(_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(_container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_clear_flag(_container, LV_OBJ_FLAG_SCROLLABLE);

    _status_label = lv_label_create(_container);
    lv_label_set_text(_status_label, "--");
    lv_obj_set_style_text_align(_status_label, LV_TEXT_ALIGN_CENTER, 0);
    
    const lv_font_t* font = (cols == 1) ? &lv_font_montserrat_20 : &lv_font_montserrat_12;
    lv_obj_set_style_text_font(_status_label, font, 0);

    _bar = lv_bar_create(_container);
    lv_obj_set_size(_bar, LV_PCT(80), 10);
    lv_bar_set_range(_bar, 0, 100);
    lv_obj_add_flag(_bar, LV_OBJ_FLAG_HIDDEN); // Hidden by default

    return _container;
}

void BlockerWidget::refresh() {
    if (!_blocker) return;
    _blocker->update();

    String msg = _blocker->getStatusMessage();
    lv_label_set_text(_status_label, msg.c_str());

    float progress = _blocker->getProgress();
    if (progress >= 0) {
        lv_obj_clear_flag(_bar, LV_OBJ_FLAG_HIDDEN);
        lv_bar_set_value(_bar, (int32_t)progress, LV_ANIM_ON);
    } else {
        lv_obj_add_flag(_bar, LV_OBJ_FLAG_HIDDEN);
    }

    if (_blocker->isFailed()) {
        lv_obj_set_style_bg_color(_container, _alertBgColor, 0);
        lv_obj_set_style_text_color(_status_label, lv_color_white(), 0);
    } else {
        lv_obj_set_style_bg_color(_container, _bgColor, 0);
        lv_obj_set_style_text_color(_status_label, _textColor, 0);
    }
}

void BlockerWidget::update(const Reading& reading) {
    // Optional override logic if needed, but primary data comes from refresh()
}

void BlockerWidget::applyTheme(ITheme* theme) {
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

    lv_obj_set_style_bg_color(_container, _bgColor, 0);
    lv_obj_set_style_text_color(_status_label, _textColor, 0);
    lv_obj_set_style_bg_color(_bar, toLvColor(0x7BEF), LV_PART_INDICATOR); // Soft blue for progress
}
