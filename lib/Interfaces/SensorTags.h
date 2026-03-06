#ifndef SENSOR_TAGS_H
#define SENSOR_TAGS_H

#include "SensorTypes.h"

/**
 * Utility for defining lists of tags at compile-time.
 */
template<typename... T>
struct TagList {};

/**
 * Base for continuous telemetry (Pressure, Temp, Weight)
 */
struct BaseTelemetryTag {
    using DataType = Reading;
    using Children = TagList<>; // Default: no dependents
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::NONE;
};

/**
 * Base for discrete system processes (WiFi, OTA, Warming Up)
 */
struct BaseServiceTag {
    using DataType = StatusMessage;
    using Children = TagList<>; // Default: no dependents
};

/**
 * Logical Tags for the Sensor Registry.
 * These are empty structs used purely as type markers.
 * Every Tag MUST have a unique static NAME for registry indexing.
 */
struct HeatingCycleTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::COUNTER;
    static constexpr const char* NAME = "HeatingCycles";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "STARTING", 0, false, QUANTITY),
            Reading(3.0f, "", "READY", 0, false, QUANTITY),
            Reading(0.0f, "", "STARTING", 0, false, QUANTITY),
            Reading(0.0f, "", "ERR", 0, true, QUANTITY)
        );
    }
};

struct BoilerPressureTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::PRESSURE;
    static constexpr const char* NAME = "BoilerPressure";
    using Children = TagList<HeatingCycleTag>;
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "BAR", "BOILER", 1, false, QUANTITY),
            Reading(3.0f, "BAR", "BOILER", 1, false, QUANTITY),
            Reading(0.0f, "BAR", "BOILER", 1, false, QUANTITY),
            Reading(0.0f, "BAR", "BOILER ERR", 1, true, QUANTITY)
        );
    }
};

struct BoilerTempTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TEMPERATURE;
    static constexpr const char* NAME = "BoilerTemp";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(25.0f, "C", "TEMP", 0, false, QUANTITY),
            Reading(150.0f, "C", "TEMP", 0, false, QUANTITY),
            Reading(25.0f, "C", "TEMP", 0, false, QUANTITY),
            Reading(0.0f, "C", "TEMP ERR", 0, true, QUANTITY)
        );
    }
};

struct ShotTimeTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TIME;
    static constexpr const char* NAME = "ShotTime";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "SECS", "READY", 1, false, QUANTITY),
            Reading(60.0f, "SECS", "RUNNING", 1, false, QUANTITY),
            Reading(0.0f, "SECS", "READY", 1, false, QUANTITY),
            Reading(0.0f, "SECS", "ERR", 1, true, QUANTITY)
        );
    }
};

struct WeightTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::WEIGHT;
    static constexpr const char* NAME = "Weight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "g", "WEIGHT", 1, false, QUANTITY),
            Reading(2000.0f, "g", "WEIGHT", 1, false, QUANTITY),
            Reading(0.0f, "g", "WEIGHT", 1, false, QUANTITY),
            Reading(0.0f, "g", "WEIGHT ERR", 1, true, QUANTITY)
        );
    }
};

struct TaredWeightTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::WEIGHT;
    static constexpr const char* NAME = "TaredWeight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(-2000.0f, "g", "WEIGHT", 1, false, QUANTITY),
            Reading(2000.0f, "g", "WEIGHT", 1, false, QUANTITY),
            Reading(0.0f, "g", "WEIGHT", 1, false, QUANTITY),
            Reading(0.0f, "g", "WEIGHT ERR", 1, true, QUANTITY)
        );
    }
};

struct WiFiStrengthTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::SIGNAL_LEVEL;
    static constexpr const char* NAME = "WiFiStrength";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(-100.0f, "dBm", "WIFI", 0, false, QUANTITY),
            Reading(-30.0f, "dBm", "WIFI", 0, false, QUANTITY),
            Reading(-100.0f, "dBm", "WIFI", 0, false, QUANTITY),
            Reading(0.0f, "dBm", "WIFI ERR", 0, true, QUANTITY)
        );
    }
};

struct LastValidShotTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TIME;
    static constexpr const char* NAME = "LastValidShot";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "SECS", "LAST SHOT", 1, false, QUANTITY),
            Reading(60.0f, "SECS", "LAST SHOT", 1, false, QUANTITY),
            Reading(0.0f, "SECS", "LAST SHOT", 1, false, QUANTITY),
            Reading(0.0f, "SECS", "ERR", 1, true, QUANTITY)
        );
    }
};

struct PumpTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::BOOLEAN;
    static constexpr const char* NAME = "Pump";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false, QUANTITY),
            Reading(1.0f, "", "ON", 0, false, QUANTITY),
            Reading(0.0f, "", "OFF", 0, false, QUANTITY),
            Reading(0.0f, "", "ERR", 0, true, QUANTITY)
        );
    }
};

struct ButtonRightTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::BOOLEAN;
    static constexpr const char* NAME = "ButtonRight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false, QUANTITY),
            Reading(1.0f, "", "ON", 0, false, QUANTITY),
            Reading(0.0f, "", "OFF", 0, false, QUANTITY),
            Reading(0.0f, "", "ERR", 0, true, QUANTITY)
        );
    }
};

struct ButtonLeftTag : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::BOOLEAN;
    static constexpr const char* NAME = "ButtonLeft";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false, QUANTITY),
            Reading(1.0f, "", "ON", 0, false, QUANTITY),
            Reading(0.0f, "", "OFF", 0, false, QUANTITY),
            Reading(0.0f, "", "ERR", 0, true, QUANTITY)
        );
    }
};

struct WiFiTag : public BaseServiceTag {
    static constexpr const char* NAME = "WiFi";
    static ServiceMetadata getMetadata() {
        return ServiceMetadata(
            StatusMessage("WiFi", "CONNECTING...", -1.0f, false),        // pending
            StatusMessage("WiFi", "AUTHENTICATING...", -1.0f, false),  // active
            StatusMessage("WiFi", "CONNECTED: 192.168.1.50", 100.0f, false), // ready
            StatusMessage("WiFi", "CONNECTION FAILED", -1.0f, true)      // failed
        );
    }
};

struct OTATag : public BaseServiceTag {
    static constexpr const char* NAME = "OTA";
    static ServiceMetadata getMetadata() {
        return ServiceMetadata(
            StatusMessage("OTA Update", "LISTENING...", 100.0f, false),  // pending
            StatusMessage("OTA Update", "UPDATING: 42%", 42.0f, false),   // active
            StatusMessage("OTA Update", "UPDATE COMPLETE", 100.0f, false),// ready
            StatusMessage("OTA Update", "UPDATE FAILED", 0.0f, true)      // failed
        );
    }
};

struct WarmingUpTag : public BaseServiceTag {
    static constexpr const char* NAME = "WarmingUp";
    static ServiceMetadata getMetadata() {
        return ServiceMetadata(
            StatusMessage("Warming Up...", "Heating Cycle 1...", 5.0f, false), // pending
            StatusMessage("Warming Up...", "Heating Cycle 2...", 45.0f, false),// active
            StatusMessage("Warming Up...", "WARM", 100.0f, false),             // ready
            StatusMessage("Warming Up...", "WARMUP TIMEOUT", 0.0f, true)       // failed
        );
    }
};

#endif
