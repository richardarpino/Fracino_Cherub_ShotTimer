#include "MachineFactory.h"

MachineFactory::MachineFactory(const MachineConfig& config) 
    : _dispatcher(),
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
      _taredWeight(&_dispatcher),
      _boilerTempProc(&_dispatcher),
      _shotMonitorProc(&_dispatcher, config.debounceMs / 1000.0f),
      _safetyProc(&_dispatcher),
      _wifi(nullptr),
      _ota(nullptr),
      _warmingUpBlocker(nullptr),
      _heatingCycleProc(&_dispatcher),
      _warmingUpProc(&_dispatcher),
      _workflowEngine(nullptr),
      _startupWorkflow(nullptr),
      _dashboardWorkflow(nullptr),
      _wifiScreen(nullptr),
      _otaScreen(nullptr),
      _warmupScreen(nullptr),
      _dashScreen(nullptr),
      _config(config) {
    
    _themes = {&_defaultTheme, &_candyTheme, &_christmasTheme};

    // Register Hardware Sensors for central polling
    _dispatcher.provide<SystemUptimeReading>(&_uptimeSensor);
    _dispatcher.provide<PumpReading>(&_pumpSensor);
    _dispatcher.provide<ButtonRightReading>(&_buttonRightSensor);
    _dispatcher.provide<ButtonLeftReading>(&_buttonLeftSensor);
    _dispatcher.provide<BoilerPressureReading>(&_boilerPressure);
    _dispatcher.provide<WeightReading>(&_weightSensor);

    // Attach Reactive Processors
    _dispatcher.attachProcessor<HeatingCycleReading>(&_heatingCycleProc);
    _dispatcher.attachProcessor<WarmingUpStatus>(&_warmingUpProc);
    _dispatcher.attachProcessor<BoilerTempReading>(&_boilerTempProc);
    _dispatcher.attachProcessor<ShotTimeReading>(&_shotMonitorProc);
    _dispatcher.attachProcessor<BoilerSafetyStatus>(&_safetyProc);
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
    if (_workflowEngine) delete _workflowEngine;
    if (_startupWorkflow) delete _startupWorkflow;
    if (_dashboardWorkflow) delete _dashboardWorkflow;
    if (_wifiScreen) delete _wifiScreen;
    if (_otaScreen) delete _otaScreen;
    if (_warmupScreen) delete _warmupScreen;
    if (_dashScreen) delete _dashScreen;
}

OTAService* MachineFactory::createOTA() {
    if (!_ota) {
        _ota = new OTAService(&_dispatcher, _config.otaHostname);
    }
    return _ota;
}

WarmingUpBlocker* MachineFactory::getWarmingUpBlocker() {
    if (!_warmingUpBlocker) {
        _warmingUpBlocker = new WarmingUpBlocker(&_dispatcher);
    }
    return _warmingUpBlocker;
}

WorkflowEngine* MachineFactory::getWorkflowEngine() {
    if (!_workflowEngine) {
        _workflowEngine = new WorkflowEngine(&_dispatcher);

        // 1. Startup Sequence (Root)
        _startupWorkflow = new BasicWorkflow();
        _wifiScreen = new BlockerScreen(getWiFiSwitch());
        _otaScreen = new BlockerScreen(createOTA());
        _warmupScreen = new BlockerScreen(getWarmingUpBlocker());
        
        _startupWorkflow->addScreen(_wifiScreen);
        _startupWorkflow->addScreen(_otaScreen);
        _startupWorkflow->addScreen(_warmupScreen);
        
        _workflowEngine->setRootWorkflow(_startupWorkflow);

        // 2. Main Dashboard (Default fallback)
        _dashboardWorkflow = new BasicWorkflow();
        _dashScreen = new DashboardScreen(&_dispatcher);
        _dashboardWorkflow->addScreen(_dashScreen);
        
        _workflowEngine->setDefaultWorkflow(_dashboardWorkflow);
    }
    return _workflowEngine;
}
