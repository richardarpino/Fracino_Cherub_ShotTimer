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
    BoilerPressure* getBoilerPressure() override { return &_boilerPressure; }
    BoilerTemperature* getBoilerTemp() override { return &_boilerTemp; }
    ShotTimer* getShotTimer() override { return &_shotTimer; }

    // ISwitchProvider
    DebouncedSwitch* getPump() override { return &_pumpSw; }
    WiFiService* getWiFiSwitch() override;
    OTAService* getOTASwitch() override { return _ota; }
    OTAService* createOTA() override;
    WarmingUpBlocker* getWarmingUpBlocker() override;

    // Specialized accessors
    DigitalRawSource* getButtonInput() { return &_buttonInput; }

private:
    // Raw Sources
    ADCRawSource _pressureADC;
    DigitalRawSource _pumpInput;
    DigitalRawSource _buttonInput;

    // Switches
    HardwareSwitch _pumpHw;
    DebouncedSwitch _pumpSw;

    // Sensors
    BoilerPressure _boilerPressure;
    BoilerTemperature _boilerTemp;
    ShotTimer _shotTimer;
    WiFiService* _wifi;
    OTAService* _ota;
    WarmingUpBlocker* _warmingUpBlocker;

    // Configuration
    MachineConfig _config;

    // Themes
    DefaultTheme _defaultTheme;
    CandyTheme _candyTheme;
    ChristmasTheme _christmasTheme;
    std::vector<ITheme*> _themes;
};

#endif
