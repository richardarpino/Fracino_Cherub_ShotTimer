#ifndef SENSOR_TAGS_H
#define SENSOR_TAGS_H

/**
 * Logical Tags for the Sensor Registry.
 * These are empty structs used purely as type markers.
 */
struct BoilerPressureTag {
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
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "SECS", "LAST SHOT", 1, false),
            Reading(60.0f, "SECS", "LAST SHOT", 1, false),
            Reading(0.0f, "SECS", "LAST SHOT", 1, false),
            Reading(0.0f, "SECS", "ERR", 1, true)
        );
    }
};

#endif
