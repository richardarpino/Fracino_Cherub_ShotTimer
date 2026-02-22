#include "OTAService.h"

OTAService::OTAService(const char* hostname) 
    : _hostname(hostname), _isActive(false), _isError(false), _progress(0) {
#ifdef ARDUINO
    ArduinoOTA.setHostname(_hostname);
    
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
    _isActive = true;
#else
    // Native mock: just mark as immediately active for simplicity in smoke tests
    _isActive = true; 
#endif
}

void OTAService::update() {
    _justStarted = _isActive && !_lastActive;
    _justStopped = !_isActive && _lastActive;
    _lastActive = _isActive;

#ifdef ARDUINO
    if (_isActive) {
        ArduinoOTA.handle();
    }
#endif
}
