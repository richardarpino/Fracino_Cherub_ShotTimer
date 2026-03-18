#include "MachineFactory.h"
#include "../Logic/Workflows/BasicWorkflow.h"
#include "../Logic/Workflows/GenericScreen.h"

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
      _shotWorkflow(nullptr),
      _config(config),
      _widgetRegistry(&_dispatcher)
#if !defined(NATIVE) || defined(SIMULATOR)
      , _lvglFactory(&_widgetRegistry) 
#endif
{
    
    _themes = {&_defaultTheme, &_candyTheme, &_christmasTheme};

    // Register Widgets with Registry
    _widgetRegistry.registerWidget<SensorWidgetTag>(WidgetCompatibility(DataCategory::TELEMETRY));
    _widgetRegistry.registerWidget<GaugeWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        { PhysicalQuantity::PRESSURE, PhysicalQuantity::TEMPERATURE } 
    ));
    _widgetRegistry.registerWidget<BlockerWidgetTag>(WidgetCompatibility(DataCategory::SERVICE));
    _widgetRegistry.registerWidget<ShotTimerWidgetTag>(WidgetCompatibility(
        DataCategory::TELEMETRY,
        {}, // No specific quantities
        { ShotTimeReading::NAME } 
    ));

    // Seed whitelists for Whitelist-First architecture
    _widgetRegistry.seedAllowedTags(
        { 
            SystemUptimeReading::NAME, 
            PumpReading::NAME, 
            ButtonRightReading::NAME, 
            ButtonLeftReading::NAME, 
            BoilerPressureReading::NAME,
            WeightReading::NAME,
            HeatingCycleReading::NAME,
            BoilerTempReading::NAME,
            ShotTimeReading::NAME,
            LastValidShotReading::NAME,
            TaredWeightReading::NAME
        },
        { 
            WarmingUpStatus::NAME, 
            BoilerSafetyStatus::NAME,
            "WiFi", // OTAService/WiFi tags
            "OTA"
        }
    );

    // Register Hardware Sensors for central polling
    _dispatcher.provide<SystemUptimeReading>(&_uptimeSensor);
    _dispatcher.seed<SystemUptimeReading>();
    
    _dispatcher.provide<PumpReading>(&_pumpSensor);
    _dispatcher.seed<PumpReading>();
    
    _dispatcher.provide<ButtonRightReading>(&_buttonRightSensor);
    _dispatcher.seed<ButtonRightReading>();
    
    _dispatcher.provide<ButtonLeftReading>(&_buttonLeftSensor);
    _dispatcher.seed<ButtonLeftReading>();
    
    _dispatcher.provide<BoilerPressureReading>(&_boilerPressure);
    _dispatcher.seed<BoilerPressureReading>();
    
    _dispatcher.provide<WeightReading>(&_weightSensor);
    _dispatcher.seed<WeightReading>();

    // Seed processors too
    _dispatcher.seed<HeatingCycleReading>();
    _dispatcher.seed<WarmingUpStatus>();
    _dispatcher.seed<BoilerTempReading>();
    _dispatcher.seed<ShotTimeReading>();
    _dispatcher.seed<BoilerSafetyStatus>();
    _dispatcher.seed<LastValidShotReading>();
    _dispatcher.seed<TaredWeightReading>();

    // Attach Reactive Processors
    _dispatcher.attachProcessor<HeatingCycleReading>(&_heatingCycleProc);
    _dispatcher.attachProcessor<WarmingUpStatus>(&_warmingUpProc);
    _dispatcher.attachProcessor<BoilerTempReading>(&_boilerTempProc);
    _dispatcher.attachProcessor<ShotTimeReading>(&_shotMonitorProc);
    _dispatcher.attachProcessor<BoilerSafetyStatus>(&_safetyProc);

#if !defined(NATIVE) || defined(SIMULATOR)
    // Register Widget Creators for Late-Binding
    LVGLWidgetFactory::registerStandardCreators(_lvglFactory);
#endif
}

IWidgetFactory* MachineFactory::getWidgetFactory() {
#if !defined(NATIVE) || defined(SIMULATOR)
    return &_lvglFactory;
#else
    return nullptr;
#endif
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
    if (_shotWorkflow) delete _shotWorkflow;
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

#include "WorkflowFactory.h"

WorkflowEngine* MachineFactory::getWorkflowEngine() {
    if (!_workflowEngine) {
        _workflowEngine = new WorkflowEngine(&_dispatcher, 1500);

        _startupWorkflow = WorkflowFactory::createSystemWorkflow(&_dispatcher, getWiFiSwitch(), createOTA(), getWarmingUpBlocker());
        _dashboardWorkflow = WorkflowFactory::createDashboardWorkflow(&_dispatcher);
        _shotWorkflow = WorkflowFactory::createShotWorkflow(&_dispatcher);

        _workflowEngine->setRootWorkflow(_startupWorkflow);
        _workflowEngine->setDefaultWorkflow(_dashboardWorkflow);
        _workflowEngine->addTriggerWorkflow(_shotWorkflow, &_pumpRegSw, 100);
    }
    return _workflowEngine;
}
