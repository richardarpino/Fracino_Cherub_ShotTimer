#include "StartupLogic.h"

StartupLogic::StartupLogic(ISwitch* wifi, IOTAService* ota, unsigned long holdDurationMs) 
    : _wifi(wifi), _ota(ota), _state(State::SEARCHING_WIFI), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false), _startTime(0), _holdDurationMs(holdDurationMs) {}

void StartupLogic::update() {
    if (!_wifi || !_ota) return;

    // Preserve previous active state for edge detection
    _lastActive = _isActive;

    switch (_state) {
        case State::SEARCHING_WIFI:
            if (_wifi->isActive()) {
                _startTime = millis();
                _state = State::WIFI_STABLE;
            }
            break;

        case State::WIFI_STABLE:
            if (!_wifi->isActive()) {
                _state = State::SEARCHING_WIFI;
                _startTime = 0;
            } else if (millis() - _startTime >= _holdDurationMs) {
                _ota->begin(OTA_HOSTNAME);
                _state = State::OTA_STARTING;
            }
            break;

        case State::OTA_STARTING:
            if (!_wifi->isActive()) {
                _state = State::SEARCHING_WIFI;
            } else if (_ota->isReady()) {
                _state = State::READY;
                _isActive = true;
            }
            break;

        case State::READY:
            if (!_wifi->isActive()) {
                _isActive = false;
                _state = State::SEARCHING_WIFI;
            }
            break;
    }

    // Logic Frame digital edge logic
    _justStarted = _isActive && !_lastActive;
    _justStopped = !_isActive && _lastActive;
}

bool StartupLogic::isActive() const { return _isActive; }
bool StartupLogic::justStarted() const { return _justStarted; }
bool StartupLogic::justStopped() const { return _justStopped; }

