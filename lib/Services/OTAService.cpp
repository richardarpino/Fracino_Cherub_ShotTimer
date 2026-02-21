#include "OTAService.h"

OTAService::OTAService() : _isActive(false), _isError(false), _progress(0) {}

void OTAService::begin(const char* hostname) {
#ifdef ARDUINO
    if (_isReady) return;
    
    ArduinoOTA.setHostname(hostname);
    
    ArduinoOTA.onStart([this]() {
        _isActive = true;
    });
    
    ArduinoOTA.onEnd([this]() {
        _isActive = false;
    });
    
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        _progress = (progress / (total / 100.0f));
    });
    
    ArduinoOTA.onError([this](ota_error_t error) {
        _isError = true;
        _isActive = false;
    });
    
    ArduinoOTA.begin();
    _isReady = true;
#else
    // Native mock: just mark as immediately active for simplicity in smoke tests
    // although we usually use the stub for real tests.
    _isReady = true; 
#endif
}

void OTAService::update() {
#ifdef ARDUINO
    if (_isReady) {
        ArduinoOTA.handle();
    }
#endif
}
