#include "WiFiService.h"

WiFiService::WiFiService(ISensorRegistry* registry, const char* ssid, const char* password) 
    : _ssid(ssid), _password(password), _isBegun(ssid != nullptr), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false), _registry(registry) {
    // Note: Registry storing wifi state as a logical reading
    if (_registry) {
        _registry->publish<WiFiTag>(Reading(0.0f, "", "DISCONNECTED", 0, false));
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
        _registry->publish<WiFiTag>(Reading(_isActive ? 1.0f : 0.0f, "", _isActive ? "CONNECTED" : "DISCONNECTED", 0, status == WL_CONNECT_FAILED));
    }
}

BlockerStatus WiFiService::getStatus() const {
    wl_status_t status = WiFi.status();
    String msg = "CONNECTING...";
    bool failed = (status == WL_CONNECT_FAILED);
    float progress = _isActive ? 100.0f : -1.0f;

    if (_isActive) {
        msg = "CONNECTED: " + WiFi.localIP().toString();
    } else if (failed) {
        msg = "CONNECTION FAILED";
    }

    return BlockerStatus("WiFi", msg, progress, failed);
}
