#include "SensorDispatcher.h"

SensorDispatcher::SensorDispatcher() {}

void SensorDispatcher::update() {
    for (size_t i = 0; i < _sensors.size(); ++i) {
        if (_sensors[i]) {
            _cache[i] = _sensors[i]->getReading();
        }
    }
}

Reading SensorDispatcher::getReadingByIndex(int index) {
    if (index < 0 || (size_t)index >= _cache.size()) {
        return Reading(0.0f, "", "", 1, true); // Unregistered
    }
    return _cache[index];
}

void SensorDispatcher::ensureCapacity(int index) {
    if ((size_t)index >= _sensors.size()) {
        _sensors.resize(index + 1, nullptr);
        _cache.resize(index + 1, Reading(0.0f, "", "", 1, true));
    }
}
