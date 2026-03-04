#ifndef SENSOR_DISPATCHER_H
#define SENSOR_DISPATCHER_H

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/HardwareSensor.h"
#include <map>
#include <string>

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
    void provide(HardwareSensor* sensor) {
        _sensors[T::NAME] = sensor;
    }

    void update() override;

protected:
    Reading getReadingByName(const char* name) override;
    void setReadingByName(const char* name, Reading reading) override;

private:
    // List of physical sensors mapped by their Type-Name
    std::map<std::string, HardwareSensor*> _sensors;
    
    // Cache of the latest readings, updated once per update() call
    std::map<std::string, Reading> _cache;
};

#endif
