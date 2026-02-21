#include "ShotDisplay.h"

ShotDisplay::ShotDisplay(ITheme* initialTheme) 
    : _tft(TFT_eSPI()), _currentTheme(initialTheme), _layout(nullptr) {}

void ShotDisplay::init() {
    _tft.init();
    _tft.setRotation(1); // Landscape
    _tft.setSwapBytes(false); // LVGL handles byte swapping via LV_COLOR_16_SWAP
    _tft.fillScreen(_currentTheme->getBackgroundColor());

    // LVGL Initialization
    lv_init();

    // Buffer Allocation
    _buf = (lv_color_t*) malloc(BUF_SIZE * sizeof(lv_color_t));
    lv_disp_draw_buf_init(&_draw_buf, _buf, NULL, BUF_SIZE);

    // Driver Registration
    lv_disp_drv_init(&_disp_drv);
    _disp_drv.hor_res = SCREEN_WIDTH;
    _disp_drv.ver_res = SCREEN_HEIGHT;
    _disp_drv.flush_cb = my_disp_flush;
    _disp_drv.draw_buf = &_draw_buf;
    _disp_drv.user_data = &_tft; 
    lv_disp_drv_register(&_disp_drv);

    // Layout Initialization
    _layout = new ScreenLayout();
    _layout->init(lv_scr_act());
    _layout->applyTheme(_currentTheme);
}

// Static Flush Callback
void ShotDisplay::my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    TFT_eSPI* tft = (TFT_eSPI*) disp->user_data;
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft->startWrite();
    tft->setAddrWindow(area->x1, area->y1, w, h);
    tft->pushColors((uint16_t *)&color_p->full, w * h, false); // No double swap
    tft->endWrite();

    lv_disp_flush_ready(disp);
}

void ShotDisplay::setTheme(ITheme* newTheme) {
    _currentTheme = newTheme;
    if (_layout) {
        _layout->applyTheme(_currentTheme);
    }
}

void ShotDisplay::update() {
    if (_layout) _layout->update();
}

void ShotDisplay::showInfo(const String& topText, const String& bottomText) {
    if (_layout) {
        String combined = topText + "\n" + bottomText;
        _layout->showMessage(combined);
    }
}

void ShotDisplay::clearScreen() {
    // No-op or just refresh theme
    if (_layout) _layout->applyTheme(_currentTheme);
}

void ShotDisplay::resetLayout(uint8_t cols, uint8_t rows) {
    if (_layout) {
        _layout->reset();
        _layout->init(lv_scr_act(), cols, rows);
        _layout->applyTheme(_currentTheme);
    }
}
