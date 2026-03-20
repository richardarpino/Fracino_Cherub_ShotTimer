#include "OTAService.h"

OTAService::OTAService(ISensorRegistry* registry, const char* hostname) 
    : _hostname(hostname), _isActive(false), _isError(false), _progress(100.0f), _registry(registry) {
    if (_registry) {
        _registry->publish<OTAStatus>(StatusMessage("OTA", "OFF", 100.0f, false));
    }
#ifdef ARDUINO
#ifdef VERBOSE_BOOT
    Serial.print("[BOOT] Starting OTA: ");
    Serial.println(_hostname);
#endif
    ArduinoOTA.setHostname(_hostname);
    
    ArduinoOTA.onStart([this]() {
#ifdef VERBOSE_BOOT
        Serial.println("[OTA] Update Started (Bridge)");
#endif
        _isError = false;
        _progress = 0;
    });
    
    ArduinoOTA.onEnd([this]() {
#ifdef VERBOSE_BOOT
        Serial.println("[OTA] Update Finished");
#endif
        _progress = 100.0f;
    });
    
    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        if (total > 0) {
            _progress = (progress / (total / 100.0f));
            
#ifdef VERBOSE_BOOT
            static int lastLoggedP = -1;
            int p = (int)_progress;
            if (p % 10 == 0 && p != lastLoggedP) { 
                Serial.print("[OTA] Progress: ");
                Serial.print(p);
                Serial.println("%");
                lastLoggedP = p;
            }
#endif
            
            // Throttle UI/Logic updates to 5Hz (200ms) to avoid starving the CPU
            if (millis() - _lastHeartbeatMillis > 200) {
                if (_registry) {
                    _registry->publish<OTAStatus>(getStatus());
                }
                if (_heartbeat) _heartbeat();
                _lastHeartbeatMillis = millis();
            }
        }
    });
    
    ArduinoOTA.onError([this](ota_error_t error) {
#ifdef VERBOSE_BOOT
        Serial.print("[OTA] Error: ");
        Serial.println(error);
#endif
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
#ifdef ARDUINO
    if (_isActive) {
        ArduinoOTA.handle();
    }
#endif

    if (_registry) {
        _registry->publish<OTAStatus>(getStatus());
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
