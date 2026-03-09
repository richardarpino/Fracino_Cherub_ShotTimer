#ifndef MACHINE_FACTORY_H
#define MACHINE_FACTORY_H

#include "../Interfaces/IMachineProvider.h"
#include "../Hardware/ADCRawSource.h"
#include "../Hardware/DigitalRawSource.h"
#include "../Sensors/Hardware/DigitalSensor.h"
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
#include "../Services/WiFiService.h"
#include "../Services/OTAService.h"
#include "../Logic/Processors/HeatingCycleProcessor.h"
#include "../Logic/Processors/WarmingUpProcessor.h"
#include "../Services/WarmingUpBlocker.h"
#include "../Logic/SensorDispatcher.h"
#include "../Interfaces/SensorTags.h"
#include "../Themes/DefaultTheme.h"
#include "../Themes/CandyTheme.h"
#include "../Themes/ChristmasTheme.h"
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

    // ISwitchProvider
    ISwitch* getPump() override { return &_pumpRegSw; }
    WiFiService* getWiFiSwitch() override;
    OTAService* getOTASwitch() override { return _ota; }
    OTAService* createOTA() override;
    WarmingUpBlocker* getWarmingUpBlocker() override;
    ISwitch* getButtonRight() override { return &_buttonRightRegSw; }
    ISwitch* getButtonLeft() override { return &_buttonLeftRegSw; }

    TaredWeightProcessor* getTaredWeight() { return &_taredWeight; }

    WorkflowEngine* getWorkflowEngine();

private:
    SensorDispatcher _dispatcher;

    // Raw Sources
    ADCRawSource _pressureADC;
    DigitalRawSource _pumpInput;
    DigitalRawSource _buttonRightInput;
    DigitalRawSource _buttonLeftInput;

    // Hardware Input Sensors (Publish to Registry)
    DigitalSensor _pumpSensor;
    DigitalSensor _buttonRightSensor;
    DigitalSensor _buttonLeftSensor;

    // Registry-Connected Switches (Consume from Registry)
    RegistrySwitch<PumpReading> _pumpRegSw;
    RegistrySwitch<ButtonRightReading> _buttonRightRegSw;
    RegistrySwitch<ButtonLeftReading> _buttonLeftRegSw;

    // Physical Sensors (Registered with Dispatcher)
    BoilerPressure _boilerPressure;
    WeightSensor _weightSensor;

    // Logical Processors (Reactive)
    TaredWeightProcessor _taredWeight;
    BoilerTemperatureProcessor _boilerTempProc;
    ShotMonitorProcessor _shotMonitorProc;
    BoilerSafetyProcessor _safetyProc;

    WiFiService* _wifi;
    OTAService* _ota;
    WarmingUpBlocker* _warmingUpBlocker;
    SystemTimeSensor _uptimeSensor;
    HeatingCycleProcessor _heatingCycleProc;
    WarmingUpProcessor _warmingUpProc;
 
    WorkflowEngine* _workflowEngine;
    BasicWorkflow* _startupWorkflow;
    BasicWorkflow* _dashboardWorkflow;
    
    IScreen* _wifiScreen;
    IScreen* _otaScreen;
    IScreen* _warmupScreen;
    IScreen* _dashScreen;
    
    // Shot Workflow
    IWorkflow* _shotWorkflow;
    IScreen* _shotScreen;

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
