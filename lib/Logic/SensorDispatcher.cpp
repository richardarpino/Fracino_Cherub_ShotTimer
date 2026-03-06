#include "SensorDispatcher.h"

SensorDispatcher::SensorDispatcher() {}

SensorDispatcher::~SensorDispatcher() {
    for (auto p : _pollers) {
        delete p;
    }
}

void SensorDispatcher::update() {
    for (auto p : _pollers) {
        p->run();
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
    _cache[name] = Reading(status.progress, "", status.message, 0, status.isFailed);
}

bool SensorDispatcher::hasProcessor(const char* name) {
    return _processors.find(name) != _processors.end();
}

void SensorDispatcher::attachProcessorInternal(const char* targetTagName, ITagProcessor* processor) {
    _processors[targetTagName] = processor;
}

void SensorDispatcher::triggerResolution(const char* name) {
    auto it = _processors.find(name);
    if (it != _processors.end()) {
        it->second->update();
    }
}
