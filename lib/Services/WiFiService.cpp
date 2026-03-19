#include "WiFiService.h"

#ifdef ARDUINO
#include <WiFi.h>
#else
#include "../../../test/_common/stubs/WiFi.h"
#endif

WiFiService::WiFiService(ISensorRegistry* registry, const char* ssid, const char* password) 
    : _ssid(ssid), _password(password), _isBegun(ssid != nullptr), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false), _registry(registry) {
    
    if (_isBegun) {
#ifdef VERBOSE_BOOT
        Serial.print("[BOOT] Starting WiFi: ");
        Serial.println(ssid);
#endif
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        
        if (_registry) {
            // Initial raw state
            _registry->publish<WiFiRawReading>(StatusMessage("WiFi", "DISCONNECTED", (float)WL_DISCONNECTED, false));
        }

        delay(100);
        WiFi.begin(ssid, password);
    }
}

void WiFiService::update() {
    wl_status_t status = WiFi.status();
    _isActive = (status == WL_CONNECTED);

    _justStarted = _isActive && !_lastActive;
    _justStopped = !_isActive && _lastActive;
    _lastActive = _isActive;

    if (_registry) {
        // Publish raw status and IP for the processor to consume
        if (_isActive) {
            strncpy(_statusBuffer, WiFi.localIP().toString().c_str(), sizeof(_statusBuffer) - 1);
            _statusBuffer[sizeof(_statusBuffer) - 1] = '\0';
        } else {
            strcpy(_statusBuffer, "");
        }
        _registry->publish<WiFiRawReading>(StatusMessage("RAW", _statusBuffer, (float)status, false));
    }
}
