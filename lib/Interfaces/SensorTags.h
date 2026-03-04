#ifndef SENSOR_TAGS_H
#define SENSOR_TAGS_H

#include "SensorTypes.h"

/**
 * Logical Tags for the Sensor Registry.
 * These are empty structs used purely as type markers.
 * Every Tag MUST have a unique static NAME for registry indexing.
 */
struct BoilerPressureTag {
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

struct BoilerTempTag {
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

struct ShotTimeTag {
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

struct WeightTag {
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

struct TaredWeightTag {
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

struct WiFiStrengthTag {
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

struct LastValidShotTag {
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

struct PumpTag {
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

struct ButtonRightTag {
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

struct ButtonLeftTag {
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

struct WiFiTag {
    static constexpr const char* NAME = "WiFi";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "DISCONNECTED", 0, false),
            Reading(1.0f, "", "CONNECTED", 0, false),
            Reading(0.0f, "", "DISCONNECTED", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
        );
    }
};

struct OTATag {
    static constexpr const char* NAME = "OTA";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false),
            Reading(1.0f, "", "ON", 0, false),
            Reading(0.0f, "", "OFF", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
        );
    }
};

struct WarmingUpTag {
    static constexpr const char* NAME = "WarmingUp";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "WARMING UP", 0, false),
            Reading(1.0f, "", "READY", 0, false),
            Reading(0.0f, "", "WARMING UP", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
        );
    }
};

#endif
