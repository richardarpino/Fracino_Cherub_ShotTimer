#ifndef SENSOR_DISPATCHER_H
#define SENSOR_DISPATCHER_H

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/HardwareSensor.h"
#include "../Interfaces/ITagProcessor.h"
#include "../Interfaces/SensorTags.h"
#include <map>
#include <string>
#include <vector>

/**
 * Concrete implementation of the Sensor Registry.
 * Manages a list of hardware sensors and provides frame-synchronized data.
 */
class SensorDispatcher : public ISensorRegistry {
public:
    SensorDispatcher();
    virtual ~SensorDispatcher();

    // Internal Polling helper
    struct IPollTask {
        virtual ~IPollTask() = default;
        virtual void run() = 0;
    };

    template<typename T>
    struct PollTask : public IPollTask {
        SensorDispatcher* d;
        HardwareSensor* s;
        PollTask(SensorDispatcher* dispatcher, HardwareSensor* sensor) : d(dispatcher), s(sensor) {}
        void run() override { 
            if (s) {
                Reading r = s->getReading();
                d->publish<T>(r.value, r.isError);
            }
        }
    };

    /**
     * Registers a physical sensor for a specific Type-Tag.
     * Creates a type-aware polling task that triggers reactive propagation.
     */
    template<typename T>
    void provide(HardwareSensor* sensor) {
        _sensors[T::NAME] = sensor;
        _pollers.push_back(new PollTask<T>(this, sensor));
    }

    void update() override;

protected:
    void triggerResolution(const char* name) override;

protected:
    Reading getReadingByName(const char* name) override;
    void setReadingByName(const char* name, Reading reading) override;
    StatusMessage getStatusByName(const char* name) override;
    void setStatusByName(const char* name, StatusMessage status) override;
    void attachProcessorInternal(const char* targetTagName, class ITagProcessor* processor) override;

private:
    std::map<std::string, HardwareSensor*> _sensors;
    std::map<std::string, Reading> _cache;
    std::map<std::string, StatusMessage> _statusCache;
    std::map<std::string, class ITagProcessor*> _processors;
    std::vector<IPollTask*> _pollers;
};

#endif
