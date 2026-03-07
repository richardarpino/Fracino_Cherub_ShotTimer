#ifndef SENSOR_TAGS_H
#define SENSOR_TAGS_H

#include "SensorTypes.h"
#include "PhysicalUnits.h"

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

// Forward declarations for reactive Children lists
struct HeatingCycleReading;
struct BoilerPressureReading;
struct BoilerTempReading;
struct ShotTimeReading;
struct WeightReading;
struct TaredWeightReading;
struct WiFiStrengthReading;
struct LastValidShotReading;
struct PumpReading;
struct ButtonRightReading;
struct ButtonLeftReading;
struct WiFiStatus;
struct OTAStatus;
struct WarmingUpStatus;
struct BoilerSafetyStatus;
struct SystemUptimeReading;

/**
 * Logical Tags for the Sensor Registry.
 * These are empty structs used purely as type markers.
 * Every Tag MUST have a unique static NAME for registry indexing.
 */
struct HeatingCycleReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::COUNTER;
    static constexpr const char* NAME = "HeatingCycles";
    using Children = TagList<struct WarmingUpStatus>;
    static SensorMetadata getMetadata() {
        return Units::Counter.range("CYCLES", 0.0f, 5.0f);
    }
};

struct SystemUptimeReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TIME;
    static constexpr const char* NAME = "Uptime";
    static SensorMetadata getMetadata() {
        return Units::Time.range("UPTIME", 0.0f, 86400.0f); // 24h range for UI if needed
    }
};

struct BoilerPressureReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::PRESSURE;
    static constexpr const char* NAME = "BoilerPressure";
    using Children = TagList<HeatingCycleReading, BoilerTempReading, BoilerSafetyStatus>;
    static SensorMetadata getMetadata() {
        return Units::Pressure.range("BOILER", 0.0f, 3.0f);
    }
};

struct BoilerTempReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TEMPERATURE;
    static constexpr const char* NAME = "BoilerTemp";
    static SensorMetadata getMetadata() {
        return Units::Temperature.range("TEMP", 25.0f, 150.0f);
    }
};

struct ShotTimeReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TIME;
    static constexpr const char* NAME = "ShotTime";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Units::Time.make(0.0f, "READY"),
            Units::Time.make(60.0f, "RUNNING"),
            Units::Time.make(0.0f, "READY"),
            Units::Time.make(0.0f, "ERR", true)
        );
    }
};

struct WeightReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::WEIGHT;
    static constexpr const char* NAME = "Weight";
    static SensorMetadata getMetadata() {
        return Units::Weight.range("WEIGHT", 0.0f, 2000.0f);
    }
};

struct TaredWeightReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::WEIGHT;
    static constexpr const char* NAME = "TaredWeight";
    static SensorMetadata getMetadata() {
        return Units::Weight.range("WEIGHT", -2000.0f, 2000.0f);
    }
};

struct WiFiStrengthReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::SIGNAL_LEVEL;
    static constexpr const char* NAME = "WiFiStrength";
    static SensorMetadata getMetadata() {
        return Units::Signal.range("WIFI", -100.0f, -30.0f);
    }
};

struct LastValidShotReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::TIME;
    static constexpr const char* NAME = "LastValidShot";
    static SensorMetadata getMetadata() {
        return Units::Time.range("LAST SHOT", 0.0f, 60.0f);
    }
};

struct PumpReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::BOOLEAN;
    static constexpr const char* NAME = "Pump";
    using Children = TagList<ShotTimeReading, TaredWeightReading>;
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Units::Boolean.make(0.0f, "OFF"),
            Units::Boolean.make(1.0f, "ON"),
            Units::Boolean.make(0.0f, "OFF"),
            Units::Boolean.make(0.0f, "ERR", true)
        );
    }
};

struct ButtonRightReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::BOOLEAN;
    static constexpr const char* NAME = "ButtonRight";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Units::Boolean.make(0.0f, "OFF"),
            Units::Boolean.make(1.0f, "ON"),
            Units::Boolean.make(0.0f, "OFF"),
            Units::Boolean.make(0.0f, "ERR", true)
        );
    }
};

struct ButtonLeftReading : public BaseTelemetryTag {
    static constexpr PhysicalQuantity QUANTITY = PhysicalQuantity::BOOLEAN;
    static constexpr const char* NAME = "ButtonLeft";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Units::Boolean.make(0.0f, "OFF"),
            Units::Boolean.make(1.0f, "ON"),
            Units::Boolean.make(0.0f, "OFF"),
            Units::Boolean.make(0.0f, "ERR", true)
        );
    }
};

struct WiFiStatus : public BaseServiceTag {
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

struct OTAStatus : public BaseServiceTag {
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

struct WarmingUpStatus : public BaseServiceTag {
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

struct BoilerSafetyStatus : public BaseServiceTag {
    static constexpr const char* NAME = "BoilerSafety";
    static ServiceMetadata getMetadata() {
        return ServiceMetadata(
            StatusMessage("Pressure", "OK", 0.0f, false),                // pending
            StatusMessage("Pressure", "DANGER: HIGH", 100.0f, false),    // active
            StatusMessage("Pressure", "OVER PRESSURE", 100.0f, true),    // ready/failed
            StatusMessage("Pressure", "SENSOR ERR", 0.0f, true)          // failed
        );
    }
};

#endif
