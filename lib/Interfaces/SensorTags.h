#ifndef SENSOR_TAGS_H
#define SENSOR_TAGS_H

#include "SensorTypes.h"

/**
 * Base for continuous telemetry (Pressure, Temp, Weight)
 */
struct BaseTelemetryTag {
    using DataType = Reading;
};

/**
 * Base for discrete system processes (WiFi, OTA, Warming Up)
 */
struct BaseServiceTag {
    using DataType = StatusMessage;
};

/**
 * Logical Tags for the Sensor Registry.
 * These are empty structs used purely as type markers.
 * Every Tag MUST have a unique static NAME for registry indexing.
 */
struct BoilerPressureTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "BoilerPressure";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "BAR", "BOILER", 1, false),
            Reading(3.0f, "BAR", "BOILER", 1, false),
            Reading(0.0f, "BAR", "BOILER", 1, false),
            Reading(0.0f, "BAR", "BOILER ERR", 1, true)
        );
    }
};

struct BoilerTempTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "BoilerTemp";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(25.0f, "C", "TEMP", 0, false),
            Reading(150.0f, "C", "TEMP", 0, false),
            Reading(25.0f, "C", "TEMP", 0, false),
            Reading(0.0f, "C", "TEMP ERR", 0, true)
        );
    }
};

struct ShotTimeTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "ShotTime";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "SECS", "READY", 1, false),
            Reading(60.0f, "SECS", "RUNNING", 1, false),
            Reading(0.0f, "SECS", "READY", 1, false),
            Reading(0.0f, "SECS", "ERR", 1, true)
        );
    }
};

struct WeightTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "Weight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "g", "WEIGHT", 1, false),
            Reading(2000.0f, "g", "WEIGHT", 1, false),
            Reading(0.0f, "g", "WEIGHT", 1, false),
            Reading(0.0f, "g", "WEIGHT ERR", 1, true)
        );
    }
};

struct TaredWeightTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "TaredWeight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(-2000.0f, "g", "WEIGHT", 1, false),
            Reading(2000.0f, "g", "WEIGHT", 1, false),
            Reading(0.0f, "g", "WEIGHT", 1, false),
            Reading(0.0f, "g", "WEIGHT ERR", 1, true)
        );
    }
};

struct WiFiStrengthTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "WiFiStrength";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(-100.0f, "dBm", "WIFI", 0, false),
            Reading(-30.0f, "dBm", "WIFI", 0, false),
            Reading(-100.0f, "dBm", "WIFI", 0, false),
            Reading(0.0f, "dBm", "WIFI ERR", 0, true)
        );
    }
};

struct LastValidShotTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "LastValidShot";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "SECS", "LAST SHOT", 1, false),
            Reading(60.0f, "SECS", "LAST SHOT", 1, false),
            Reading(0.0f, "SECS", "LAST SHOT", 1, false),
            Reading(0.0f, "SECS", "ERR", 1, true)
        );
    }
};

struct PumpTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "Pump";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false),
            Reading(1.0f, "", "ON", 0, false),
            Reading(0.0f, "", "OFF", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
        );
    }
};

struct ButtonRightTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "ButtonRight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false),
            Reading(1.0f, "", "ON", 0, false),
            Reading(0.0f, "", "OFF", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
        );
    }
};

struct ButtonLeftTag : public BaseTelemetryTag {
    static constexpr const char* NAME = "ButtonLeft";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false),
            Reading(1.0f, "", "ON", 0, false),
            Reading(0.0f, "", "OFF", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
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
