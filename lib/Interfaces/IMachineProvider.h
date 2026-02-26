#ifndef IMACHINE_PROVIDER_H
#define IMACHINE_PROVIDER_H

#include <vector>
#include "ISensor.h"
#include "ISwitch.h"
#include "IBlocker.h"
#include "ISensorRegistry.h"

/**
 * Global Configuration for the Machine.
 * Injected into the Factory to decouple credentials and hardware constants from interfaces.
 */
struct MachineConfig {
    const char* otaHostname;
    const char* wifiSsid;
    const char* wifiPassword;
    unsigned long debounceMs;
};

class ITheme;

/**
 * Interface Segregation: Provides access to machine-wide sensors.
 */
class ISensorProvider {
public:
    virtual ~ISensorProvider() = default;
    virtual ISensorRegistry* getRegistry() = 0;
};

/**
 * Interface Segregation: Provides access to machine-wide switches and logic gates.
 */
class ISwitchProvider {
public:
    virtual ~ISwitchProvider() = default;
    virtual ISwitch* getPump() = 0;
    virtual IBlocker* getWiFiSwitch() = 0;
    virtual IBlocker* getOTASwitch() = 0;
    virtual IBlocker* createOTA() = 0;
    virtual IBlocker* getWarmingUpBlocker() = 0;
    virtual ISwitch* getButtonRight() = 0;
    virtual ISwitch* getButtonLeft() = 0;
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
