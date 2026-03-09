#ifndef WIDGET_TAGS_H
#define WIDGET_TAGS_H

#include <stdint.h>

struct SensorWidgetTag {
    static constexpr const char* NAME = "Sensor";
    static constexpr uint8_t MIN_COLS = 1;
    static constexpr uint8_t MIN_ROWS = 1;
    static constexpr bool ASPECT_LOCKED = false;
};

struct GaugeWidgetTag {
    static constexpr const char* NAME = "Gauge";
    static constexpr uint8_t MIN_COLS = 1;
    static constexpr uint8_t MIN_ROWS = 1;
    static constexpr bool ASPECT_LOCKED = true;
};

struct ShotTimerWidgetTag {
    static constexpr const char* NAME = "ShotTimer";
    static constexpr uint8_t MIN_COLS = 1;
    static constexpr uint8_t MIN_ROWS = 1;
    static constexpr bool ASPECT_LOCKED = true;
};

struct BlockerWidgetTag {
    static constexpr const char* NAME = "Blocker";
    static constexpr uint8_t MIN_COLS = 1;
    static constexpr uint8_t MIN_ROWS = 1;
    static constexpr bool ASPECT_LOCKED = false;
};

#endif
