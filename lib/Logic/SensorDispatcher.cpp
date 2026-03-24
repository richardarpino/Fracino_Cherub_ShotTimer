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
    if (it != _cache.end()) return it->second;
    
    // Fallback to init value from metadata if available
    auto metaIt = _sensorMetadata.find(name);
    if (metaIt != _sensorMetadata.end()) {
        return metaIt->second.init;
    }
    
    return Reading(0.0f, "", "", 1, true); 
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
    storeMetadataInternal(name, ServiceMetadata(status, status, status, status)); // Default transition
    _statusCache[name] = status;
    _cache[name] = Reading(status.progress, "", status.message, 0, status.isFailed);
}

SensorMetadata SensorDispatcher::getSensorMetadataByName(const char* name) {
    auto it = _sensorMetadata.find(name);
    if (it != _sensorMetadata.end()) return it->second;
    return SensorMetadata();
}

ServiceMetadata SensorDispatcher::getServiceMetadataByName(const char* name) {
    auto it = _serviceMetadata.find(name);
    if (it != _serviceMetadata.end()) return it->second;
    return ServiceMetadata();
}

void SensorDispatcher::storeMetadataInternal(const char* name, SensorMetadata meta) {
    _sensorMetadata[name] = meta;
}

void SensorDispatcher::storeMetadataInternal(const char* name, ServiceMetadata meta) {
    _serviceMetadata[name] = meta;
}

bool SensorDispatcher::hasProcessor(const char* name) {
    return _processors.find(name) != _processors.end();
}

void SensorDispatcher::attachProcessorInternal(const char* targetTagName, ITagProcessor* processor) {
    _processors.insert({targetTagName, processor});
}

void SensorDispatcher::triggerResolution(const char* name) {
    auto range = _processors.equal_range(name);
    for (auto it = range.first; it != range.second; ++it) {
        it->second->update();
    }
}

DataCategory SensorDispatcher::getCategory(const char* name) {
    if (_statusCache.find(name) != _statusCache.end()) return DataCategory::SERVICE;
    return DataCategory::TELEMETRY; // Default
}

PhysicalQuantity SensorDispatcher::getQuantity(const char* name) {
    auto it = _cache.find(name);
    if (it != _cache.end()) return it->second.quantity;
    return PhysicalQuantity::NONE;
}
