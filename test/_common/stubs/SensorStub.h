#ifndef SENSOR_STUB_H
#define SENSOR_STUB_H

#include "../../lib/Interfaces/ISensor.h"

class SensorStub : public ISensor {
public:
    void setReading(const Reading& reading) { _reading = reading; }
    Reading getReading() override { return _reading; }
    SensorMetadata getMetadata() override { return SensorMetadata(); }
    void update() {}

private:
    Reading _reading = Reading(0.0f, "UNIT", "LABEL", 1, false);
};

#endif
