#include "MachineFactory.h"

MachineFactory::MachineFactory(const MachineConfig& config) 
    : _config(config),
      _pressureADC(pressurePin),
      _pumpInput(pumpPin),
      _buttonRightInput(buttonRightPin),
      _buttonLeftInput(buttonLeftPin),
      _pumpSensor(&_pumpInput, true, config.debounceMs),
      _buttonRightSensor(&_buttonRightInput, true, config.debounceMs),
      _buttonLeftSensor(&_buttonLeftInput, true, config.debounceMs),
      _pumpRegSw(&_dispatcher),
      _buttonRightRegSw(&_dispatcher),
      _buttonLeftRegSw(&_dispatcher),
      _boilerPressure(&_pressureADC, pressureScalar),
      _weightSensor(nullptr), 
      _boilerTemp(&_boilerPressure),
      _manualPumpTimer(),
      _taredWeight(&_weightSensor),
      _wifi(nullptr),
      _ota(nullptr),
      _warmingUpBlocker(nullptr),
      _heatingCycleProc(&_dispatcher) {
    
    _themes = {&_defaultTheme, &_candyTheme, &_christmasTheme};

    // Register Hardware Sensors for central polling
    _dispatcher.provide<PumpTag>(&_pumpSensor);
    _dispatcher.provide<ButtonRightTag>(&_buttonRightSensor);
    _dispatcher.provide<ButtonLeftTag>(&_buttonLeftSensor);
    _dispatcher.provide<BoilerPressureTag>(&_boilerPressure);
    _dispatcher.provide<WeightTag>(&_weightSensor);

    // Attach Reactive Processors
    _dispatcher.attachProcessor<HeatingCycleTag>(&_heatingCycleProc);
}

WiFiService* MachineFactory::getWiFiSwitch() {
    if (!_wifi) {
        _wifi = new WiFiService(&_dispatcher, _config.wifiSsid, _config.wifiPassword);
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
        _ota = new OTAService(&_dispatcher, _config.otaHostname);
    }
    return _ota;
}

WarmingUpBlocker* MachineFactory::getWarmingUpBlocker() {
    if (!_warmingUpBlocker) {
        _warmingUpBlocker = new WarmingUpBlocker(&_dispatcher, &_boilerPressure);
    }
    return _warmingUpBlocker;
}
