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
        return getReadingByIndex(getTypeIndex<T>());
    }

    /**
     * Publishes a logical reading to the registry cache.
     * Used by orchestrators to provide non-physical data.
     */
    template<typename T>
    void publish(Reading reading) {
        setReadingByIndex(getTypeIndex<T>(), reading);
    }

    /**
     * Returns the static metadata for a specific Type-Tag.
     */
    template<typename T>
    SensorMetadata getMetadata() {
        return getMetadataByIndex(getTypeIndex<T>());
    }

protected:
    // Internal bridging to allow templates to work with virtual methods
    virtual Reading getReadingByIndex(int index) = 0;
    virtual void setReadingByIndex(int index, Reading reading) = 0;
    virtual SensorMetadata getMetadataByIndex(int index) = 0;
    
    // Static type-to-index mapping trick
    static int getNextIndex() {
        static int nextIndex = 0;
        return nextIndex++;
    }

    template<typename T>
    static int getTypeIndex() {
        static int index = getNextIndex();
        return index;
    }
};

#endif
