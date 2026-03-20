#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H

#include "../Interfaces/IBlocker.h"

#ifdef ARDUINO
#include <ArduinoOTA.h>
#else
// No-op for native
#endif

#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"
#include <functional>

class OTAService {
public:
    OTAService(ISensorRegistry* registry, const char* hostname);
    virtual ~OTAService() = default;
    
    // Background Service Implementation
    void update();
    const char* getTagName() const { return OTAStatus::NAME; }

    StatusMessage getStatus() const;

    void setHeartbeat(std::function<void()> heartbeat) { _heartbeat = heartbeat; }

private:
    ISensorRegistry* _registry;
    const char* _hostname;
    bool _isActive = false;
    bool _isError = false;
    float _progress = 100.0f;
    mutable char _statusBuffer[64];
    std::function<void()> _heartbeat = nullptr;
    unsigned long _lastHeartbeatMillis = 0;
};

#endif
