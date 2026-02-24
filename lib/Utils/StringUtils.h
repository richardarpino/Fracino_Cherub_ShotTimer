#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <Arduino.h>

namespace StringUtils {
    /**
     * Formats a float to a string with 1 decimal place.
     * Use this instead of snprintf %f which is not supported in all environments.
     */
    inline void formatFloat1(char* buf, size_t bufSize, float val) {
        int whole = (int)val;
        int frac = (int)((val - whole) * 10 + 0.5f);
        if (frac >= 10) { whole++; frac = 0; }
        snprintf(buf, bufSize, "%d.%d", whole, frac);
    }
}

#endif
