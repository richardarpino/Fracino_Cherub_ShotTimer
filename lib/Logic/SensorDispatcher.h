#ifndef SENSOR_DISPATCHER_H
#define SENSOR_DISPATCHER_H

#include "../Interfaces/ISensorRegistry.h"
#include <vector>

/**
 * Concrete implementation of the Sensor Registry.
 * Manages a list of hardware sensors and provides frame-synchronized data.
 */
class SensorDispatcher : public ISensorRegistry {
public:
    SensorDispatcher();

    /**
     * Registers a physical sensor for a specific Type-Tag.
     */
    template<typename T>
    void provide(ISensor* sensor) {
        int index = getTypeIndex<T>();
        ensureCapacity(index);
        _sensors[index] = sensor;
    }

    void update() override;

protected:
    Reading getReadingByIndex(int index) override;

private:
    void ensureCapacity(int index);

    // List of physical sensors mapped by their Type-Index
    std::vector<ISensor*> _sensors;
    
    // Cache of the latest readings, updated once per update() call
    std::vector<Reading> _cache;
};

#endif
