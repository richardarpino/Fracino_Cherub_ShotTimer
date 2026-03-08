#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include "../Interfaces/IBlocker.h"

#ifdef ARDUINO
#include <WiFi.h>
#else
#include "../../test/_common/stubs/WiFi.h"
#endif

#include "../Interfaces/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"

class WiFiService : public IBlocker {
public:
    WiFiService(ISensorRegistry* registry, const char* ssid = nullptr, const char* password = nullptr);
    
    // IBlocker Implementation
    StatusMessage getStatus() const override;
    const char* getTagName() const override { return WiFiStatus::NAME; }

    // ISwitch Implementation
    void update() override;
    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _justStarted; }
    bool justStopped() const override { return _justStopped; }

private:
    ISensorRegistry* _registry;
    const char* _ssid;
   const char* _password;
   bool _isBegun;
   bool _isActive;
   bool _justStarted;
   bool _justStopped;
   bool _lastActive;
   mutable char _statusBuffer[64];
};

#endif
