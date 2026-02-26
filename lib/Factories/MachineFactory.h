#ifndef MACHINE_FACTORY_H
#define MACHINE_FACTORY_H

#include "../Interfaces/IMachineProvider.h"
#include "../Hardware/ADCRawSource.h"
#include "../Hardware/DigitalRawSource.h"
#include "../Sensors/Hardware/HardwareSwitch.h"
#include "../Sensors/Virtual/DebouncedSwitch.h"
#include "../Sensors/Hardware/BoilerPressure.h"
#include "../Sensors/Hardware/WeightSensor.h"
#include "../Logic/BoilerTemperature.h"
#include "../Logic/ManualPumpTimer.h"
#include "../Logic/TaredWeight.h"
#include "../Services/WiFiService.h"
#include "../Services/OTAService.h"
#include "../Services/WarmingUpBlocker.h"
#include "../Logic/SensorDispatcher.h"
#include "../Interfaces/SensorTags.h"
#include "../Themes/DefaultTheme.h"
#include "../Themes/CandyTheme.h"
#include "../Themes/ChristmasTheme.h"
#include "../../include/pins.h"

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

    // ISwitchProvider
    DebouncedSwitch* getPump() override { return &_pumpSw; }
    WiFiService* getWiFiSwitch() override;
    OTAService* getOTASwitch() override { return _ota; }
    OTAService* createOTA() override;
    WarmingUpBlocker* getWarmingUpBlocker() override;
    ISwitch* getButtonRight() override { return &_buttonRightSw; }
    ISwitch* getButtonLeft() override { return &_buttonLeftSw; }

    // Component Access for Orchestrators
    BoilerPressure* getBoilerPressure() { return &_boilerPressure; }
    WeightSensor* getWeightSensor() { return &_weightSensor; }
    BoilerTemperature* getBoilerTemp() { return &_boilerTemp; }
    ManualPumpTimer* getManualPumpTimer() { return &_manualPumpTimer; }
    TaredWeight* getTaredWeight() { return &_taredWeight; }

private:
    // Raw Sources
    ADCRawSource _pressureADC;
    DigitalRawSource _pumpInput;
    DigitalRawSource _buttonRightInput;
    DigitalRawSource _buttonLeftInput;

    // Switches
    HardwareSwitch _pumpHw;
    DebouncedSwitch _pumpSw;
    HardwareSwitch _buttonRightHw;
    DebouncedSwitch _buttonRightSw;
    HardwareSwitch _buttonLeftHw;
    DebouncedSwitch _buttonLeftSw;

    // Physical Sensors (Registered with Dispatcher)
    BoilerPressure _boilerPressure;
    WeightSensor _weightSensor;

    // Logical Components (Published to Dispatcher via ScaleLogic)
    BoilerTemperature _boilerTemp;
    ManualPumpTimer _manualPumpTimer;
    TaredWeight _taredWeight;

    WiFiService* _wifi;
    OTAService* _ota;
    WarmingUpBlocker* _warmingUpBlocker;
    SensorDispatcher _dispatcher;

    MachineConfig _config;

    DefaultTheme _defaultTheme;
    CandyTheme _candyTheme;
    ChristmasTheme _christmasTheme;
    std::vector<ITheme*> _themes;
};

#endif
