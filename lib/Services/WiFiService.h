#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include "../Interfaces/IBlocker.h"

#ifdef ARDUINO
#include <WiFi.h>
#else
#include "../../test/_common/stubs/WiFi.h"
#endif

class WiFiService : public IBlocker {
public:
    WiFiService(const char* ssid = nullptr, const char* password = nullptr);
    
    // IBlocker Implementation
    BlockerStatus getStatus() const override;

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
