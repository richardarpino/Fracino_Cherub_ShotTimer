#include "WiFiSensor.h"

WiFiSensor::WiFiSensor() 
    : _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false) {}

void WiFiSensor::begin(const char* ssid, const char* password) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin(ssid, password);
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
