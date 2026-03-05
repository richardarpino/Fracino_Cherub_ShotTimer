#include "WiFiService.h"

WiFiService::WiFiService(ISensorRegistry* registry, const char* ssid, const char* password) 
    : _ssid(ssid), _password(password), _isBegun(ssid != nullptr), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false), _registry(registry) {
    // Note: Registry storing wifi state as a logical reading
    if (_registry) {
        _registry->publish<WiFiTag>(StatusMessage("WiFi", "DISCONNECTED", -1.0f, false));
    }
    
    if (_isBegun) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
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
        _registry->publish<WiFiTag>(getStatus());
    }
}

StatusMessage WiFiService::getStatus() const {
    wl_status_t status = WiFi.status();
    const char* title = "WiFi";
    const char* msg = "CONNECTING...";
    bool failed = (status == WL_CONNECT_FAILED);
    float progress = _isActive ? 100.0f : -1.0f;

    if (_isActive) {
        snprintf(_statusBuffer, sizeof(_statusBuffer), "CONNECTED: %s", WiFi.localIP().toString().c_str());
        msg = _statusBuffer;
    } else if (failed) {
        msg = "CONNECTION FAILED";
    }

    return StatusMessage(title, msg, progress, failed);
}
