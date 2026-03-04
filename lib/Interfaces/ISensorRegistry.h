#ifndef ISENSOR_REGISTRY_H
#define ISENSOR_REGISTRY_H

#include "SensorTypes.h"

/**
 * Interface for the centralized sensor data registry.
 * Provides a type-safe way to retrieve the latest cached readings.
 */
class ISensorRegistry {
public:
    virtual ~ISensorRegistry() = default;

    /**
     * Updates all registered sensors and caches their latest readings.
     * Should be called once per main loop.
     */
    virtual void update() = 0;

    template<typename T>
    Reading getLatest() {
        return getReadingByName(T::NAME);
    }

    /**
     * Publishes a logical reading to the registry cache.
     * Used by orchestrators to provide non-physical data.
     */
    template<typename T>
    void publish(Reading reading) {
        setReadingByName(T::NAME, reading);
    }

    /**
     * Returns the static metadata for a specific Type-Tag.
     */
    template<typename T>
    SensorMetadata getMetadata() {
        return T::getMetadata();
    }

protected:
    // Internal bridging to allow templates to work with virtual methods
    virtual Reading getReadingByName(const char* name) = 0;
    virtual void setReadingByName(const char* name, Reading reading) = 0;
};

#endif
