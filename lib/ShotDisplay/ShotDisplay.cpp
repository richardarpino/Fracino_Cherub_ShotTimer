#include "ShotDisplay.h"

ShotDisplay::ShotDisplay() 
    : _tft(TFT_eSPI()), _currentTheme(nullptr), _externalLayout(nullptr) {}

void ShotDisplay::init() {
    _tft.init();
    _tft.setRotation(1); // Landscape
    _tft.setSwapBytes(false); // LVGL handles byte swapping via LV_COLOR_16_SWAP
    
    if (_currentTheme) {
        _tft.fillScreen(_currentTheme->getBackgroundColor());
    } else {
        _tft.fillScreen(TFT_BLACK);
    }
 
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

void ShotDisplay::setLayout(ScreenLayout* layout) {
    if (_externalLayout == layout) return;
    
    _externalLayout = layout;
    if (_externalLayout) {
        lv_obj_clean(lv_scr_act()); // Remove old widgets
        _externalLayout->init(lv_scr_act()); // Default 2x2 or as configured in layout
        _externalLayout->applyTheme(_currentTheme);
    }
}
 
void ShotDisplay::setTheme(ITheme* newTheme) {
    _currentTheme = newTheme;
    if (_externalLayout) {
        _externalLayout->applyTheme(_currentTheme);
    }
}
 
void ShotDisplay::update() {
    if (_externalLayout) _externalLayout->update();
}
 
void ShotDisplay::clearScreen() {
    if (_externalLayout) _externalLayout->applyTheme(_currentTheme);
}
