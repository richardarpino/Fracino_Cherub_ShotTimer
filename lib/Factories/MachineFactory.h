#ifndef MACHINE_FACTORY_H
#define MACHINE_FACTORY_H

#include "../Interfaces/IMachineProvider.h"
#include "../Hardware/ADCRawSource.h"
#include "../Hardware/DigitalRawSource.h"
#include "../Sensors/Hardware/HardwareSwitch.h"
#include "../Sensors/Virtual/DebouncedSwitch.h"
#include "../Sensors/Hardware/BoilerPressure.h"
#include "../Sensors/Virtual/BoilerTemperature.h"
#include "../Sensors/Hardware/ShotTimer.h"
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
 * Manages the lifetime and configuration of all physical and virtual machine components.
 * Returns specialized types to allow control logic access (Covariant Returns).
 */
class MachineFactory : public ISensorProvider, public ISwitchProvider, public IThemeProvider {
public:
    MachineFactory(const MachineConfig& config);
    ~MachineFactory();

    // IThemeProvider
    const std::vector<ITheme*>& getThemes() const override { return _themes; }

    // ISensorProvider
    ShotTimer* getShotTimer() override { return &_shotTimer; }
    ISensorRegistry* getRegistry() override { return &_dispatcher; }

    // ISwitchProvider
    DebouncedSwitch* getPump() override { return &_pumpSw; }
    WiFiService* getWiFiSwitch() override;
    OTAService* getOTASwitch() override { return _ota; }
    OTAService* createOTA() override;
    WarmingUpBlocker* getWarmingUpBlocker() override;
    ISwitch* getButtonRight() override { return &_buttonRightSw; }
    ISwitch* getButtonLeft() override { return &_buttonLeftSw; }

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

    // Sensors
    BoilerPressure _boilerPressure;
    BoilerTemperature _boilerTemp;
    ShotTimer _shotTimer;
    WiFiService* _wifi;
    OTAService* _ota;
    WarmingUpBlocker* _warmingUpBlocker;
    SensorDispatcher _dispatcher;

    // Configuration
    MachineConfig _config;

    // Themes
    DefaultTheme _defaultTheme;
    CandyTheme _candyTheme;
    ChristmasTheme _christmasTheme;
    std::vector<ITheme*> _themes;
};

#endif
