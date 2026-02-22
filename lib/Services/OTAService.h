#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H

#include "../Interfaces/ISensor.h"
#include "../Interfaces/ISwitch.h"

#ifdef ARDUINO
#include <ArduinoOTA.h>
#else
// No-op for native
#endif

class OTAService : public ISensor, public ISwitch {
public:
    OTAService(const char* hostname);
    
    void update() override;
    
    Reading getReading() override { return { _progress, "%" }; }
    
    // ISwitch implementation
    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _justStarted; }
    bool justStopped() const override { return _justStopped; }

private:
    const char* _hostname;
    bool _isActive = false;
    bool _lastActive = false;
    bool _justStarted = false;
    bool _justStopped = false;
    bool _isError = false;
    float _progress = 0;
};

#endif
