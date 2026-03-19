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
      _wifiService(nullptr),
      _wifiProc(&_dispatcher),
      _wifiBlocker(&_dispatcher),
      _otaService(nullptr),
      _otaBlocker(&_dispatcher),
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
    BOOT_LOG("001", "Registering Widgets...");
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

    BOOT_LOG("002", "Seeding Whitelists...");
    // Seed whitelists for Whitelist-First architecture
    _widgetRegistry.applyTypeWhitelists(AllowedSensors{}, AllowedServices{});

    BOOT_LOG("003", "Wiring Hardware Sensors...");
    // Register Hardware Sensors for central polling
    _dispatcher.provide<SystemUptimeReading>(&_uptimeSensor);
    _dispatcher.provide<PumpReading>(&_pumpSensor);
    _dispatcher.provide<ButtonRightReading>(&_buttonRightSensor);
    _dispatcher.provide<ButtonLeftReading>(&_buttonLeftSensor);
    _dispatcher.provide<BoilerPressureReading>(&_boilerPressure);
    _dispatcher.provide<WeightReading>(&_weightSensor);

    // Apply global whitelists to the Dispatcher (Auto-seeds all metadata)
    _dispatcher.applyTypeWhitelists(AllowedSensors{});
    _dispatcher.applyTypeWhitelists(AllowedServices{});

    BOOT_LOG("004", "Attaching Reactive Processors...");
    // Attach Reactive Processors
    _dispatcher.attachProcessor<HeatingCycleReading>(&_heatingCycleProc);
    _dispatcher.attachProcessor<WarmingUpStatus>(&_warmingUpProc);
    _dispatcher.attachProcessor<BoilerTempReading>(&_boilerTempProc);
    _dispatcher.attachProcessor<ShotTimeReading>(&_shotMonitorProc);
    _dispatcher.attachProcessor<WiFiRawReading>(&_wifiProc);
    _dispatcher.attachProcessor<BoilerSafetyStatus>(&_safetyProc);

#if !defined(NATIVE) || defined(SIMULATOR)
    BOOT_LOG("005", "Initializing UI Widget Factory...");
    // Register Widget Creators for Late-Binding
    LVGLWidgetFactory::registerStandardCreators(_lvglFactory);
#endif

    BOOT_LOG("011", "Registering Themes...");
    _themes.push_back(&_defaultTheme);
    _themes.push_back(&_candyTheme);
    _themes.push_back(&_christmasTheme);
}

void MachineFactory::BOOT_LOG(const char* code, const char* msg) {
#ifndef NATIVE
    if (_config.verboseBoot) {
        Serial.print("[BOOT] ");
        Serial.print(code);
        Serial.print(": ");
        Serial.println(msg);
    }
#endif
}

IWidgetFactory* MachineFactory::getWidgetFactory() {
#if !defined(NATIVE) || defined(SIMULATOR)
    return &_lvglFactory;
#else
    return nullptr;
#endif
}

IBlocker* MachineFactory::getWiFiSwitch() {
    if (!_wifiService) {
        _wifiService = new WiFiService(&_dispatcher, _config.wifiSsid, _config.wifiPassword);
    }
    return &_wifiBlocker;
}

MachineFactory::~MachineFactory() {
    if (_wifiService) delete _wifiService;
    if (_otaService) delete _otaService;
    if (_warmingUpBlocker) delete _warmingUpBlocker;
    if (_workflowEngine) delete _workflowEngine;
    if (_startupWorkflow) delete _startupWorkflow;
    if (_dashboardWorkflow) delete _dashboardWorkflow;
    if (_shotWorkflow) delete _shotWorkflow;
}

IBlocker* MachineFactory::createOTA() {
    if (!_otaService) {
        _otaService = new OTAService(&_dispatcher, _config.otaHostname);
    }
    return &_otaBlocker;
}

IBlocker* MachineFactory::getWarmingUpBlocker() {
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

void MachineFactory::update() {
    // 1. Update background services (Active logic)
    if (_wifiService) _wifiService->update();
    if (_otaService) _otaService->update();

    // 2. Update blockers (Passive polling)
    _wifiBlocker.update();
    _otaBlocker.update();
    if (_warmingUpBlocker) _warmingUpBlocker->update();
}
