#ifndef MACHINE_FACTORY_H
#define MACHINE_FACTORY_H

#include "../Interfaces/IMachineProvider.h"
#include "../Hardware/ADCRawSource.h"
#include "../Hardware/DigitalRawSource.h"
#include "../Hardware/HX711Source.h"
#include "../Sensors/Hardware/DigitalSensor.h"
#include "../Sensors/Hardware/RawHardwareSensor.h"
#include "../Sensors/Registry/RegistrySwitch.h"
#include "../Sensors/Hardware/BoilerPressure.h"
#include "../Sensors/Hardware/WeightSensor.h"
#include "../Sensors/Hardware/SystemTimeSensor.h"
#include "../Logic/Processors/TaredWeightProcessor.h"
#include "../Logic/Processors/BoilerTemperatureProcessor.h"
#include "../Logic/Processors/ShotMonitorProcessor.h"
#include "../Logic/Processors/BoilerSafetyProcessor.h"
#include "../Logic/Workflows/WorkflowEngine.h"
#include "../Logic/Workflows/BasicWorkflow.h"
#include "../Logic/Triggers/OTADownloadingTrigger.h"
#include "../Services/WiFiService.h"
#include "../Services/OTAService.h"
#include "../Services/OTABlocker.h"
#include "../Logic/Processors/HeatingCycleProcessor.h"
#include "../Logic/Processors/WarmingUpProcessor.h"
#include "../Services/WarmingUpBlocker.h"
#include "../Logic/Processors/WiFiProcessor.h"
#include "../Services/WiFiBlocker.h"
#include "../Logic/Triggers/DelayedTrigger.h"
#include "../Logic/SensorDispatcher.h"
#include "../Logic/Triggers/WorkflowRunningTrigger.h"
#include "../Interfaces/SensorTags.h"
#include "../Logic/Processors/WeightCalibrationProcessor.h"
#include "../Themes/DefaultTheme.h"
#include "../Themes/CandyTheme.h"
#include "../Themes/ChristmasTheme.h"
#include "../Interfaces/IWidgetFactory.h"
#include "../../include/pins.h"

#include "../Registry/WidgetRegistry.h"
#include "../Registry/WidgetTags.h"
#if !defined(NATIVE) || defined(SIMULATOR)
#include "LVGLWidgetFactory.h"
#endif

/**
 * Concrete Machine Factory.
 * Lifetime manager for all machine components.
 */
class MachineFactory : public ISensorProvider, public ISwitchProvider, public IThemeProvider {
public:
    MachineFactory(const MachineConfig& config);
    ~MachineFactory();

    // IThemeProvider
    const std::vector<ITheme*>& getThemes() const override { return _themes; }

    // ISensorProvider
    ISensorRegistry* getRegistry() override { return &_dispatcher; }
    IWidgetRegistry* getWidgetRegistry() { return &_widgetRegistry; }
    IWidgetFactory* getWidgetFactory();

    // ISwitchProvider
    ISwitch* getPump() override { return &_pumpRegSw; }
    IBlocker* getWiFiSwitch() override;
    IBlocker* getOTASwitch() override { return &_otaBlocker; }
    IBlocker* createOTA() override;
    IBlocker* getWarmingUpBlocker() override;
    ISwitch* getButtonRight() override { return &_buttonRightRegSw; }
    ISwitch* getButtonLeft() override { return &_buttonLeftRegSw; }

    TaredWeightProcessor* getTaredWeight() { return &_taredWeight; }

    WorkflowEngine* getWorkflowEngine();
    void update();
    void setHeartbeat(std::function<void()> heartbeat);

private:
    void BOOT_LOG(const char* code, const char* msg);

private:
    SensorDispatcher _dispatcher;

    // Raw Sources
    ADCRawSource _pressureADC;
    DigitalRawSource _pumpInput;
    DigitalRawSource _buttonRightInput;
    DigitalRawSource _buttonLeftInput;
    HX711Source _hx711Input;

    // Hardware Input Sensors (Publish to Registry)
    DigitalSensor _pumpSensor;
    DigitalSensor _buttonRightSensor;
    DigitalSensor _buttonLeftSensor;
    RawHardwareSensor _rawWeightSensor;

    // Registry-Connected Switches (Consume from Registry)
    RegistrySwitch<PumpReading> _pumpRegSw;
    RegistrySwitch<ButtonRightReading> _buttonRightRegSw;
    RegistrySwitch<ButtonLeftReading> _buttonLeftRegSw;

    // Physical Sensors (Registered with Dispatcher)
    BoilerPressure _boilerPressure;
    WeightSensor _weightSensor;

    // Logical Processors (Reactive)
    WeightCalibrationProcessor _weightCalibProc;
    TaredWeightProcessor _taredWeight;
    BoilerTemperatureProcessor _boilerTempProc;
    ShotMonitorProcessor _shotMonitorProc;
    BoilerSafetyProcessor _safetyProc;

    WiFiService* _wifiService;
    WiFiProcessor _wifiProc;
    WiFiBlocker _wifiBlocker;
    OTAService* _otaService;
    OTABlocker _otaBlocker;
    WarmingUpBlocker* _warmingUpBlocker;
    SystemTimeSensor _uptimeSensor;
    HeatingCycleProcessor _heatingCycleProc;
    WarmingUpProcessor _warmingUpProc;
 
    WorkflowEngine* _workflowEngine;
    IWorkflow* _startupWorkflow;
    IWorkflow* _dashboardWorkflow;
    IWorkflow* _shotWorkflow;
    IWorkflow* _otaUpdateWorkflow;
    OTADownloadingTrigger* _otaDownloadingTrigger;
    DelayedTrigger* _shotSummaryTrigger;
    WorkflowRunningTrigger* _startupRunningTrigger;
    ITrigger* _alwaysTrigger;

    MachineConfig _config;

    WidgetRegistry _widgetRegistry;
#if !defined(NATIVE) || defined(SIMULATOR)
    LVGLWidgetFactory _lvglFactory;
#endif

    DefaultTheme _defaultTheme;
    CandyTheme _candyTheme;
    ChristmasTheme _christmasTheme;
    std::vector<ITheme*> _themes;
};

#endif
