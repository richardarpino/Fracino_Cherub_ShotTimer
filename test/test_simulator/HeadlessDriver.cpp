#include "HeadlessDriver.h"
#include <Arduino.h>
#include "lv_snapshot.h"
#include <fstream>
#include <iostream>

uint32_t HeadlessDriver::_width = 0;
uint32_t HeadlessDriver::_height = 0;

void HeadlessDriver::init(uint32_t width, uint32_t height) {
    _width = width;
    _height = height;
    
    lv_init();

    // Allocate draw buffer
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buf1[240 * 320]; 
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, width * height);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = flush_cb;
    disp_drv.hor_res = width;
    disp_drv.ver_res = height;
    lv_disp_drv_register(&disp_drv);
}

void HeadlessDriver::flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
    lv_disp_flush_ready(disp_drv);
}

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
};

struct BMPInfo {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

void HeadlessDriver::saveSnapshot(lv_obj_t* obj, const std::string& filename) {
    // Process layout and any pending tasks
    addMillis(100);
    lv_timer_handler();
    
    uint32_t w = lv_obj_get_width(obj);
    uint32_t h = lv_obj_get_height(obj);
    
    // Take snapshot using LVGL's snapshot tool
    lv_img_dsc_t* snap = lv_snapshot_take(obj, LV_IMG_CF_TRUE_COLOR_ALPHA);
    if (!snap) {
        std::cerr << "Failed to take snapshot!" << std::endl;
        return;
    }

    std::ofstream f(filename, std::ios::binary);
    if (!f) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        lv_snapshot_free(snap);
        return;
    }

    BMPHeader header = {0x4D42, uint32_t(sizeof(BMPHeader) + sizeof(BMPInfo) + (w * h * 4)), 0, 0, uint32_t(sizeof(BMPHeader) + sizeof(BMPInfo))};
    BMPInfo info = {sizeof(BMPInfo), (int32_t)w, (int32_t)-h, 1, 32, 0, (uint32_t)(w * h * 4), 2835, 2835, 0, 0};

    f.write((char*)&header, sizeof(header));
    f.write((char*)&info, sizeof(info));

    const uint8_t* data = snap->data;
    for (uint32_t i = 0; i < w * h; i++) {
        uint8_t a = 255; // Force opaque for BMP compatibility
        uint8_t r = data[i * 4 + 2];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 0];
        f.put(b); f.put(g); f.put(r); f.put(a);
    }

    f.close();
    lv_snapshot_free(snap);
}
