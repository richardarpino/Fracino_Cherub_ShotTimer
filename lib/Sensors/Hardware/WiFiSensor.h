#ifndef WIFI_SENSOR_H
#define WIFI_SENSOR_H

#include "../../Interfaces/ISensor.h"
#include "../../Interfaces/ISwitch.h"

#ifdef ARDUINO
#include <WiFi.h>
#else
#include "../../../test/_common/stubs/WiFi.h"
#endif

class WiFiSensor : public ISensor, public ISwitch {
public:
    WiFiSensor(const char* ssid = nullptr, const char* password = nullptr);
    
    // ISensor Implementation
    Reading getReading() override;
    SensorMetadata getMetadata() override;

    // ISwitch Implementation
    void update() override;
    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _justStarted; }
    bool justStopped() const override { return _justStopped; }

private:
   const char* _ssid;
   const char* _password;
   bool _isBegun;
   bool _isActive;
   bool _justStarted;
   bool _justStopped;
   bool _lastActive;
};

#endif
