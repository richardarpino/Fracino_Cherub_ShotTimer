#ifndef SHOT_DISPLAY_H
#define SHOT_DISPLAY_H

#include <TFT_eSPI.h>
#include "../Themes/ITheme.h"
#include "../Themes/IThemeable.h"
#include "../Interfaces/SensorTypes.h"

#include <lvgl.h>


#include "../UI/ScreenLayout.h"

class ShotDisplay : public IThemeable {
public:
    ShotDisplay();
    void init();
    
    // IThemeable implementation
    void setTheme(ITheme* newTheme) override;

    // Screens (Status Widget)
    void showInfo(const String& topText, const String& bottomText);
    void clearScreen();
    void resetLayout(uint8_t cols = 2, uint8_t rows = 2);
    void setRegistry(class ISensorRegistry* registry);

    // Updates
    void update(); // The main "Pull" update
    
    ScreenLayout* getLayout() { return _layout; }
    
    // LVGL Flush Callback (Must be static to be a C callback)
    static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

private:
    TFT_eSPI _tft;
    ITheme* _currentTheme;
    ScreenLayout* _layout;

    // LVGL buffers and driver
    static const uint16_t SCREEN_WIDTH = TFT_HEIGHT; // 240 in landscape
    static const uint16_t SCREEN_HEIGHT = TFT_WIDTH; // 135 in landscape
    static const uint32_t BUF_SIZE = SCREEN_WIDTH * 10; // 1/10th of screen
    lv_disp_draw_buf_t _draw_buf;
    lv_color_t* _buf; // Dyn alloc or static array is fine
    lv_disp_drv_t _disp_drv;
};

#endif
