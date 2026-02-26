#ifndef SENSOR_STUB_H
#define SENSOR_STUB_H

#include "../../lib/Interfaces/SensorTypes.h"
#include "../../lib/Interfaces/HardwareSensor.h"

class SensorStub : public HardwareSensor {
public:
    SensorStub() : HardwareSensor(nullptr) {}
    void setReading(const Reading& reading) { _reading = reading; }
    Reading getReading() override { return _reading; }
    void update() {}

private:
    Reading _reading = Reading(0.0f, "UNIT", "LABEL", 1, false);
};

#endif
