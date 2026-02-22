#include "MachineFactory.h"

MachineFactory::MachineFactory(const MachineConfig& config) 
    : _config(config),
      _pressureADC(pressurePin),
      _pumpInput(pumpPin),
      _buttonInput(buttonPin),
      _pumpHw(&_pumpInput, true),
      _pumpSw(&_pumpHw, config.debounceMs),
      _boilerPressure(&_pressureADC, pressureScalar),
      _boilerTemp(&_boilerPressure),
      _shotTimer(config.minShotDuration),
      _wifi(nullptr),
      _ota(nullptr) {
    _themes.push_back(&_defaultTheme);
    _themes.push_back(&_candyTheme);
    _themes.push_back(&_christmasTheme);
}

WiFiSensor* MachineFactory::getWiFiSensor() {
    if (!_wifi) {
        _wifi = new WiFiSensor(_config.wifiSsid, _config.wifiPassword);
    }
    return _wifi;
}

WiFiSensor* MachineFactory::getWiFiSwitch() {
    return getWiFiSensor();
}

MachineFactory::~MachineFactory() {
    if (_wifi) delete _wifi;
    if (_ota) delete _ota;
}

OTAService* MachineFactory::createOTA() {
    if (!_ota) {
        _ota = new OTAService(_config.otaHostname);
    }
    return _ota;
}
