#ifndef IMACHINE_PROVIDER_H
#define IMACHINE_PROVIDER_H

#include <vector>
#include "ISensor.h"
#include "ISwitch.h"

/**
 * Global Configuration for the Machine.
 * Injected into the Factory to decouple credentials and hardware constants from interfaces.
 */
struct MachineConfig {
    const char* otaHostname;
    const char* wifiSsid;
    const char* wifiPassword;
    unsigned long debounceMs;
    float minShotDuration;
};

class ITheme;

/**
 * Interface Segregation: Provides access to machine-wide sensors.
 */
class ISensorProvider {
public:
    virtual ~ISensorProvider() = default;
    virtual ISensor* getBoilerPressure() = 0;
    virtual ISensor* getBoilerTemp() = 0;
    virtual ISensor* getShotTimer() = 0;
    virtual ISensor* getOTASensor() = 0;
    virtual ISensor* getWiFiSensor() = 0;
};

/**
 * Interface Segregation: Provides access to machine-wide switches and logic gates.
 */
class ISwitchProvider {
public:
    virtual ~ISwitchProvider() = default;
    virtual ISwitch* getPump() = 0;
    virtual ISwitch* getWiFiSwitch() = 0;
    virtual ISwitch* getOTASwitch() = 0;
    virtual ISwitch* createOTA() = 0;
};

/**
 * Interface Segregation: Provides access to available visual themes.
 */
class IThemeProvider {
public:
    virtual ~IThemeProvider() = default;
    virtual const std::vector<ITheme*>& getThemes() const = 0;
};

#endif
