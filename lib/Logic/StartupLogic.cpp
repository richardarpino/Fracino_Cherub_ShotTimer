#include "StartupLogic.h"
#include "../Interfaces/IMachineProvider.h"
#include "../Interfaces/IBlocker.h"

StartupLogic::StartupLogic(ISwitchProvider* provider, unsigned long holdDurationMs) 
    : _wifi(nullptr), _warmingUp(nullptr), _provider(provider), _state(State::SEARCHING_WIFI), _isActive(false), _justStarted(false), _justStopped(false), _lastActive(false), _startTime(0), _holdDurationMs(holdDurationMs) {}

BlockerStatus StartupLogic::getStatus() const {
    if (!_wifi) return BlockerStatus("Startup", "INITIALIZING...", -1.0f, false);

    switch (_state) {
        case State::SEARCHING_WIFI:
        case State::WIFI_STABLE:
            return _wifi->getStatus();
        
        case State::OTA_STARTING: {
            if (!_provider) return BlockerStatus("OTA", "ERROR", -1.0f, true);
            IBlocker* ota = _provider->getOTASwitch();
            return ota ? ota->getStatus() : BlockerStatus("OTA", "STARTING...", -1.0f, false);
        }

        case State::WARMING_UP:
            return _warmingUp ? _warmingUp->getStatus() : BlockerStatus("Warming Up", "STARTING...", -1.0f, false);

        case State::READY:
            return BlockerStatus("Ready", "SYSTEM OK", 100.0f, false);

        default:
            return BlockerStatus("Startup", "BUSY", -1.0f, false);
    }
}

void StartupLogic::update() {
    if (!_provider) return;

    // Lazily obtain WiFi on first update (triggers instantiation/hardware start)
    if (!_wifi) {
        _wifi = _provider->getWiFiSwitch();
    }
    if (!_wifi) return;

    // Logic Ownership: Orchestrator polls its constituents
    _wifi->update();
    IBlocker* ota = _provider->getOTASwitch();
    if (ota) ota->update();
    
    if (_warmingUp) _warmingUp->update();

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
                _provider->createOTA(); 
                _state = State::OTA_STARTING;
            }
            break;

        case State::OTA_STARTING:
            if (!_wifi->isActive()) {
                _state = State::SEARCHING_WIFI;
            } else {
                IBlocker* ota = _provider->getOTASwitch();
                if (ota && ota->isActive()) {
                    _warmingUp = _provider->getWarmingUpBlocker();
                    _state = State::WARMING_UP;
                }
            }
            break;

        case State::WARMING_UP:
            if (!_wifi->isActive()) {
                _state = State::SEARCHING_WIFI;
            } else if (_warmingUp && _warmingUp->isActive()) {
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

