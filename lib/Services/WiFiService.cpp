#include "WiFiService.h"

WiFiService::WiFiService(const char* ssid, const char* password) 
    : _ssid(ssid), _password(password), _isBegun(ssid != nullptr), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false) {
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
}

String WiFiService::getStatusMessage() const {
    wl_status_t status = WiFi.status();
    
    switch (status) {
        case WL_CONNECTED:
            return "CONNECTED\n" + WiFi.localIP().toString();
        case WL_CONNECT_FAILED:
            return "FAILED";
        case WL_IDLE_STATUS:
        case WL_DISCONNECTED:
            return "CONNECTING...";
        default:
            return "CONNECTING...";
    }
}

float WiFiService::getProgress() const {
    return _isActive ? 100.0f : -1.0f; // Indeterminate while connecting
}

bool WiFiService::isFailed() const {
    return WiFi.status() == WL_CONNECT_FAILED;
}
