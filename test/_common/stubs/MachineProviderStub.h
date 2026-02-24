#ifndef MACHINE_PROVIDER_STUB_H
#define MACHINE_PROVIDER_STUB_H

#include "Interfaces/IMachineProvider.h"
#include "OTAServiceStub.h"
#include "IBlocker.h"

class MachineProviderStub : public ISensorProvider, public ISwitchProvider, public IThemeProvider {
public:
    MachineProviderStub(IBlocker* wifiSwitch, OTAServiceStub* ota) 
        : _wifiSwitch(wifiSwitch), _ota(ota), _otaCreated(false) {}

    // IThemeProvider
    const std::vector<ITheme*>& getThemes() const override { return _themes; }

    // ISensorProvider
    ISensor* getBoilerPressure() override { return nullptr; }
    ISensor* getBoilerTemp() override { return nullptr; }
    ISensor* getShotTimer() override { return nullptr; }

    // ISwitchProvider
    ISwitch* getPump() override { return nullptr; }
    IBlocker* getWiFiSwitch() override { return _wifiSwitch; }
    IBlocker* getOTASwitch() override { return _ota; }
    IBlocker* createOTA() override { 
        _otaCreated = true;
        return _ota; 
    }

    bool otaCreated() const { return _otaCreated; }

private:
    IBlocker* _wifiSwitch;
    OTAServiceStub* _ota;
    bool _otaCreated;
    std::vector<ITheme*> _themes;
};

#endif
