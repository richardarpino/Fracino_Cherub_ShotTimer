#include "MachineFactory.h"

MachineFactory::MachineFactory(const MachineConfig& config) 
    : _config(config),
      _pressureADC(pressurePin),
      _pumpInput(pumpPin),
      _buttonRightInput(buttonPin),
      _buttonLeftInput(buttonLeftPin),
      _pumpHw(&_pumpInput, true),
      _pumpSw(&_pumpHw, config.debounceMs),
      _buttonRightHw(&_buttonRightInput, true),
      _buttonRightSw(&_buttonRightHw, config.debounceMs),
      _buttonLeftHw(&_buttonLeftInput, true),
      _buttonLeftSw(&_buttonLeftHw, config.debounceMs),
      _boilerPressure(&_pressureADC, pressureScalar),
      _boilerTemp(&_boilerPressure),
      _shotTimer(config.minShotDuration),
      _wifi(nullptr),
      _ota(nullptr),
      _warmingUpBlocker(nullptr) {
    _themes.push_back(&_defaultTheme);
    _themes.push_back(&_candyTheme);
    _themes.push_back(&_christmasTheme);

    // Register sensors with the dispatcher
    _dispatcher.provide<BoilerPressureTag>(&_boilerPressure);
    _dispatcher.provide<BoilerTempTag>(&_boilerTemp);
    _dispatcher.provide<ShotTimeTag>(&_shotTimer);
}

WiFiService* MachineFactory::getWiFiSwitch() {
    if (!_wifi) {
        _wifi = new WiFiService(_config.wifiSsid, _config.wifiPassword);
    }
    return _wifi;
}

MachineFactory::~MachineFactory() {
    if (_wifi) delete _wifi;
    if (_ota) delete _ota;
    if (_warmingUpBlocker) delete _warmingUpBlocker;
}

OTAService* MachineFactory::createOTA() {
    if (!_ota) {
        _ota = new OTAService(_config.otaHostname);
    }
    return _ota;
}

WarmingUpBlocker* MachineFactory::getWarmingUpBlocker() {
    if (!_warmingUpBlocker) {
        // Fudge timeout to 10s for USB-C testing (No machine pressurized)
        _warmingUpBlocker = new WarmingUpBlocker(&_boilerPressure, 10000);
    }
    return _warmingUpBlocker;
}
