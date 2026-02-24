#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H

#include "../Interfaces/IBlocker.h"

#ifdef ARDUINO
#include <ArduinoOTA.h>
#else
// No-op for native
#endif

class OTAService : public IBlocker {
public:
    OTAService(const char* hostname);
    
    // IBlocker implementation
    BlockerStatus getStatus() const override;

    // ISwitch implementation
    void update() override;
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
