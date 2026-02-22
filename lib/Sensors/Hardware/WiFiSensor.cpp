#include "WiFiSensor.h"

WiFiSensor::WiFiSensor(const char* ssid, const char* password) 
    : _ssid(ssid), _password(password), _isBegun(ssid != nullptr), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false) {
    if (_isBegun) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        delay(100);
        WiFi.begin(ssid, password);
    }
}

void WiFiSensor::update() {
    wl_status_t status = WiFi.status();
    _isActive = (status == WL_CONNECTED);

    _justStarted = _isActive && !_lastActive;
    _justStopped = !_isActive && _lastActive;
    _lastActive = _isActive;
}

Reading WiFiSensor::getReading() {
    wl_status_t status = WiFi.status();
    
    float value = (status == WL_CONNECTED) ? 1.0f : 0.0f;
    String label = "DISCONNECTED";
    bool isError = false;

    switch (status) {
        case WL_CONNECTED:
            label = "CONNECTED\n" + WiFi.localIP().toString();
            break;
        case WL_CONNECT_FAILED:
            label = "FAILED";
            isError = true;
            break;
        case WL_IDLE_STATUS:
        case WL_DISCONNECTED:
            label = "CONNECTING...";
            break;
        default:
            label = "CONNECTING...";
            break;
    }

    return Reading(value, "", label, 0, isError);
}

SensorMetadata WiFiSensor::getMetadata() {
    return SensorMetadata(
        Reading(0.0f, "", "DISCONNECTED", 0, false),
        Reading(1.0f, "", "CONNECTED", 0, false),
        Reading(0.0f, "", "SEARCHING", 0, false),
        Reading(0.0f, "", "WIFI ERR", 0, true)
    );
}
