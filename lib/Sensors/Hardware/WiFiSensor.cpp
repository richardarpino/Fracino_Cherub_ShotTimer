#include "WiFiSensor.h"

WiFiSensor::WiFiSensor() {}

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
