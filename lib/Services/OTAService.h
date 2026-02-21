#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H

#include "../Interfaces/IOTAService.h"

#ifdef ARDUINO
#include <ArduinoOTA.h>
#else
// No-op for native
#endif

class OTAService : public IOTAService {
public:
    OTAService();
    
    void begin(const char* hostname) override;
    void update() override;
    
    Reading getReading() override { return { _progress, "%" }; }
    bool isActive() const override { return _isActive; }
    bool isReady() const override { return _isReady; }
    bool isError() const override { return _isError; }

private:
    bool _isReady = false;
    bool _isActive = false;
    bool _isError = false;
    float _progress = 0;
};

#endif
