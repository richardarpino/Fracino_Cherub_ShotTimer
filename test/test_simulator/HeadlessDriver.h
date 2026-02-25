#ifndef HEADLESS_DRIVER_H
#define HEADLESS_DRIVER_H

#include <lvgl.h>
#include <vector>
#include <string>

/**
 * Native LVGL driver for headless rendering and snapshot generation.
 * Uses a memory buffer instead of a physical display.
 */
class HeadlessDriver {
public:
    static void init(uint32_t width = 135, uint32_t height = 240);
    static void saveSnapshot(lv_obj_t* obj, const std::string& filename);

private:
    static void flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p);
    
    static uint32_t _width;
    static uint32_t _height;
};

#endif
