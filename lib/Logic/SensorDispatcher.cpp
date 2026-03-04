#include "SensorDispatcher.h"

SensorDispatcher::SensorDispatcher() {}

void SensorDispatcher::update() {
    for (auto const& [name, sensor] : _sensors) {
        if (sensor) {
            _cache[name] = sensor->getReading();
        }
    }
}

Reading SensorDispatcher::getReadingByName(const char* name) {
    auto it = _cache.find(name);
    if (it == _cache.end()) {
        return Reading(0.0f, "", "", 1, true); 
    }
    return it->second;
}

void SensorDispatcher::setReadingByName(const char* name, Reading reading) {
    _cache[name] = reading;
}
