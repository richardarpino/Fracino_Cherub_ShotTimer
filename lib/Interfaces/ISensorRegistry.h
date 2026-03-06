#ifndef ISENSOR_REGISTRY_H
#define ISENSOR_REGISTRY_H

#include "SensorTypes.h"
#include <type_traits>

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

    /**
     * Returns the latest cached data for a specific Type-Tag.
     */
    template<typename T>
    typename T::DataType getLatest() {
        return getInternal<T>(typename T::DataType{});
    }

    /**
     * Publishes data to the registry cache.
     */
    template<typename T>
    void publish(typename T::DataType data) {
        publishInternal<T>(data);
    }

    /**
     * Registers a processor to be triggered when a specific tag is updated.
     */
    template<typename T>
    void attachProcessor(class ITagProcessor* processor) {
        attachProcessorInternal(T::NAME, processor);
    }

protected:
    // Internal tag-dispatching helpers
    template<typename T>
    Reading getInternal(Reading) {
        return getReadingByName(T::NAME);
    }

    template<typename T>
    StatusMessage getInternal(StatusMessage) {
        return getStatusByName(T::NAME);
    }

    template<typename T>
    void publishInternal(Reading reading) {
        setReadingByName(T::NAME, reading);
    }

    template<typename T>
    void publishInternal(StatusMessage status) {
        setStatusByName(T::NAME, status);
    }

public:
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
    virtual StatusMessage getStatusByName(const char* name) = 0;
    virtual void setStatusByName(const char* name, StatusMessage status) = 0;
    virtual void attachProcessorInternal(const char* targetTagName, class ITagProcessor* processor) = 0;
};

#endif
