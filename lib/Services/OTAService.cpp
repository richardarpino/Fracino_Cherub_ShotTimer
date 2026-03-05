#include "OTAService.h"

OTAService::OTAService(ISensorRegistry* registry, const char* hostname) 
    : _hostname(hostname), _isActive(false), _isError(false), _progress(100.0f), _registry(registry) {
    if (_registry) {
        _registry->publish<OTATag>(StatusMessage("OTA", "OFF", 100.0f, false));
    }
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
    _progress = 100.0f;
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

    if (_registry) {
        _registry->publish<OTATag>(getStatus());
    }
}

StatusMessage OTAService::getStatus() const {
    const char* title = "OTA Update";
    const char* msg = "OTA INACTIVE";
    
    if (_isError) {
        msg = "UPDATE FAILED";
    } else if (_progress > 0 && _progress < 100.0f) {
        snprintf(_statusBuffer, sizeof(_statusBuffer), "UPDATING: %d%%", (int)_progress);
        msg = _statusBuffer;
    } else if (_isActive) {
        msg = "LISTENING...";
    }
    
    return StatusMessage(title, msg, _progress, _isError);
}
