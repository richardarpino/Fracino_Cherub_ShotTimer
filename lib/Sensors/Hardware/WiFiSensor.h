#ifndef WIFI_SENSOR_H
#define WIFI_SENSOR_H

#include "../../Interfaces/ISensor.h"

#ifdef ARDUINO
#include <WiFi.h>
#else
#include "../../../test/_common/stubs/WiFi.h"
#endif

class WiFiSensor : public ISensor {
public:
    WiFiSensor();
    Reading getReading() override;
};

#endif
