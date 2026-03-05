#include "StartupLogic.h"
#include "../Interfaces/IMachineProvider.h"
#include "../Interfaces/IBlocker.h"

StartupLogic::StartupLogic(ISwitchProvider* provider, ISensorRegistry* registry, unsigned long holdDurationMs) 
    : _wifiSwitch(registry), _otaSwitch(registry), _warmingUpSwitch(registry), _wifi(nullptr), _warmingUp(nullptr), _provider(provider), _registry(registry),
      _state(State::SEARCHING_WIFI), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false), _startTime(0), _holdDurationMs(holdDurationMs) {}

StatusMessage StartupLogic::getStatus() const {
    if (!_wifi) return StatusMessage("Startup", "INITIALIZING...", -1.0f, false);

    switch (_state) {
        case State::SEARCHING_WIFI:
        case State::WIFI_STABLE:
            return _wifi->getStatus();
        
        case State::OTA_STARTING: {
            if (!_provider) return StatusMessage("OTA", "ERROR", -1.0f, true);
            IBlocker* ota = _provider->getOTASwitch();
            return ota ? ota->getStatus() : StatusMessage("OTA", "STARTING...", -1.0f, false);
        }

        case State::WARMING_UP:
            return _warmingUp ? _warmingUp->getStatus() : StatusMessage("Warming Up", "STARTING...", -1.0f, false);

        case State::READY:
            return StatusMessage("Ready", "SYSTEM OK", 100.0f, false);

        default:
            return StatusMessage("Startup", "BUSY", -1.0f, false);
    }
}

void StartupLogic::update() {
    if (!_provider || !_registry) return;

    // Lazily obtain WiFi on first update (triggers instantiation/hardware start)
    if (!_wifi) {
        _wifi = _provider->getWiFiSwitch();
    }
    if (!_wifi) return;

    _wifiSwitch.update();
    _otaSwitch.update();
    _warmingUpSwitch.update();

    // Preserve previous active state for edge detection
    _lastActive = _isActive;

    switch (_state) {
        case State::SEARCHING_WIFI:
            if (_wifiSwitch.isActive()) {
                _startTime = millis();
                _state = State::WIFI_STABLE;
            }
            break;

        case State::WIFI_STABLE:
            if (!_wifiSwitch.isActive()) {
                _state = State::SEARCHING_WIFI;
                _startTime = 0;
            } else if (millis() - _startTime >= _holdDurationMs) {
                _provider->createOTA(); 
                _state = State::OTA_STARTING;
            }
            break;

        case State::OTA_STARTING:
            if (!_wifiSwitch.isActive()) {
                _state = State::SEARCHING_WIFI;
            } else {
                if (_otaSwitch.isActive()) {
                    _warmingUp = _provider->getWarmingUpBlocker();
                    _state = State::WARMING_UP;
                }
            }
            break;

        case State::WARMING_UP:
            if (!_wifiSwitch.isActive()) {
                _state = State::SEARCHING_WIFI;
            } else if (_warmingUpSwitch.isActive()) {
                _state = State::READY;
                _isActive = true;
            }
            break;

        case State::READY:
            if (!_wifiSwitch.isActive()) {
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
