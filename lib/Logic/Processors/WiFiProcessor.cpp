#include "WiFiProcessor.h"
#include <stdio.h>

#ifdef ARDUINO
#include <WiFi.h>
#else
#include "../../../test/_common/stubs/WiFi.h"
#endif

WiFiProcessor::WiFiProcessor(ISensorRegistry* registry) : _registry(registry) {}

void WiFiProcessor::update() {
    if (!_registry) return;
    
    StatusMessage raw = _registry->getLatest<WiFiRawReading>();
    int status = (int)raw.progress;
    const char* ip = raw.message;

    const char* title = "WiFi";
    const char* msg = "CONNECTING...";
    bool failed = (status == WL_CONNECT_FAILED);
    float progress = (status == WL_CONNECTED) ? 100.0f : -1.0f;

    if (status == WL_CONNECTED) {
        snprintf(_statusBuffer, sizeof(_statusBuffer), "CONNECTED: %s", ip);
        msg = _statusBuffer;
    } else if (failed) {
        msg = "CONNECTION FAILED";
    }

    _registry->publish<WiFiStatus>(StatusMessage(title, msg, progress, failed));
}
