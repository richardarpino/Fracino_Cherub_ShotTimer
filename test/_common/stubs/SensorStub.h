#ifndef SENSOR_STUB_H
#define SENSOR_STUB_H

#include "../../lib/Interfaces/SensorTypes.h"
#include "../../lib/Interfaces/HardwareSensor.h"

class SensorStub : public HardwareSensor {
public:
    SensorStub() : HardwareSensor(nullptr) {}
    void setReading(float value) { _value = value; }
    float getReading() override { return _value; }
    void update() {}

private:
    float _value = 0.0f;
};

#endif
