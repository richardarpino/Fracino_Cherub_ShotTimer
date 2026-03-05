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

StatusMessage SensorDispatcher::getStatusByName(const char* name) {
    auto it = _statusCache.find(name);
    if (it == _statusCache.end()) {
        // Fallback to reading if no status exists
        auto rit = _cache.find(name);
        if (rit != _cache.end()) {
            return StatusMessage("", rit->second.label, rit->second.value, rit->second.isError);
        }
        return StatusMessage("Error", "Not Found", -1.0f, true);
    }
    return it->second;
}

void SensorDispatcher::setStatusByName(const char* name, StatusMessage status) {
    _statusCache[name] = status;
    
    // For compatibility with widgets that only know about Readings
    _cache[name] = Reading(status.progress, "", status.message, 0, status.isFailed);
}
