#ifndef PHYSICAL_UNITS_H
#define PHYSICAL_UNITS_H

#include "SensorTypes.h"

namespace Units {
    /**
     * Defines the formatting and identity of a physical unit.
     */
    struct Unit {
        const char* symbol;
        int precision; // Number of decimal places
        PhysicalQuantity quantity;

        // Helper to produce a standardized Reading
        Reading make(float value, const char* label, bool isError = false) const {
            return Reading(value, symbol, label, precision, isError, quantity);
        }

        // The DSL Factory: Generates standard SensorMetadata
        SensorMetadata range(const char* label, float low, float high) const {
            return SensorMetadata(
                make(low, label),          // Low state
                make(high, label),         // High state
                make(low, label),          // Default/Init state
                make(0.0f, "ERR", true)    // Error state
            );
        }
    };

    // Centralized Unit Definitions
    static constexpr Unit Pressure    = {"bar",  1, PhysicalQuantity::PRESSURE};
    static constexpr Unit Temperature = {"C",    0, PhysicalQuantity::TEMPERATURE};
    static constexpr Unit Weight      = {"g",    1, PhysicalQuantity::WEIGHT};
    static constexpr Unit Time        = {"secs", 1, PhysicalQuantity::TIME};
    static constexpr Unit Counter     = {"",     0, PhysicalQuantity::COUNTER};
    static constexpr Unit Percentage  = {"%",    0, PhysicalQuantity::POWER};
    static constexpr Unit Signal      = {"dBm",  0, PhysicalQuantity::SIGNAL_LEVEL};
    static constexpr Unit Boolean     = {"",     0, PhysicalQuantity::BOOLEAN};
}

#endif
