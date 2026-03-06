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
        resolveDerived(typename T::Children{});
    }

    /**
     * Publishes a raw numeric value, automatically wrapping it in a Reading
     * using the Tag's metadata (units, precision, quantity).
     */
    template<typename T>
    void publish(float value, bool isError = false) {
        SensorMetadata meta = T::getMetadata();
        Reading r;
        
        // Use DSL-standard metadata (units, precision, quantity)
        r.unit = meta.init.unit;
        r.precision = meta.init.precision;
        r.quantity = meta.init.quantity;
        r.value = value;
        r.isError = isError;

        // Custom labels for Boolean states
        if (r.quantity == PhysicalQuantity::BOOLEAN) {
            r.label = (value > 0.5f) ? meta.high.label : meta.low.label;
        } else {
            r.label = meta.init.label;
        }

        publish<T>(r);
    }

protected:
    template<typename... Ts>
    void resolveDerived(TagList<Ts...>) {
        int dummy[] = { 0, (triggerResolution(Ts::NAME), 0)... };
        (void)dummy;
    }

    virtual void triggerResolution(const char* name) = 0;

public:
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
        Reading reading = getReadingByName(T::NAME);
        // If not found (initial state), return the init value from metadata
        if (reading.isError && reading.unit[0] == '\0') {
            return T::getMetadata().init;
        }
        return reading;
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
