#include "OTAService.h"

OTAService::OTAService(const char* hostname) 
    : _hostname(hostname), _isActive(false), _isError(false), _progress(0) {
#ifdef ARDUINO
    ArduinoOTA.setHostname(_hostname);
    
    ArduinoOTA.onStart([this]() {
        _isError = false;
        _progress = 0;
    });
    
    ArduinoOTA.onEnd([this]() {
        _progress = 100.0f;
    });
    
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        if (total > 0) {
            _progress = (progress / (total / 100.0f));
        }
    });
    
    ArduinoOTA.onError([this](ota_error_t error) {
        _isError = true;
        _progress = 0;
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

String OTAService::getStatusMessage() const {
    if (_isError) return "OTA ERROR";
    if (_progress > 0 && _progress < 100.0f) {
        return "UPDATING: " + String((int)_progress) + "%";
    }
    if (_isActive) return "OTA READY";
    return "OTA INACTIVE";
}

float OTAService::getProgress() const {
    return _progress;
}

bool OTAService::isFailed() const {
    return _isError;
}
