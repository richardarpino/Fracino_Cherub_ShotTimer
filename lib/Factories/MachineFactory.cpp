#include "MachineFactory.h"

MachineFactory::MachineFactory(const MachineConfig& config) 
    : _config(config),
      _pressureADC(pressurePin),
      _pumpInput(pumpPin),
      _buttonRightInput(buttonRightPin),
      _buttonLeftInput(buttonLeftPin),
      _pumpHw(&_pumpInput, true),
      _pumpSw(&_pumpHw, config.debounceMs),
      _buttonRightHw(&_buttonRightInput, true),
      _buttonRightSw(&_buttonRightHw, config.debounceMs),
      _buttonLeftHw(&_buttonLeftInput, true),
      _buttonLeftSw(&_buttonLeftHw, config.debounceMs),
      _boilerPressure(&_pressureADC, pressureScalar),
      _weightSensor(nullptr), // TODO: Wire actual weight source if available
      _boilerTemp(&_boilerPressure),
      _manualPumpTimer(),
      _taredWeight(&_weightSensor),
      _wifi(nullptr),
      _ota(nullptr),
      _warmingUpBlocker(nullptr) {
    
    _themes = {&_defaultTheme, &_candyTheme, &_christmasTheme};

    // ONLY Physical Sensors are registered for polling
    _dispatcher.provide<BoilerPressureTag>(&_boilerPressure);
    _dispatcher.provide<WeightTag>(&_weightSensor);
    
    // Logical Components (BoilerTemp, ShotTime, TaredWeight) 
    // are NOT registered here. They are pushed by ScaleLogic.
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
        _warmingUpBlocker = new WarmingUpBlocker(&_boilerPressure);
    }
    return _warmingUpBlocker;
}
