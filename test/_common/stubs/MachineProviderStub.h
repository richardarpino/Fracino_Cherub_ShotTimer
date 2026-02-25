#ifndef MACHINE_PROVIDER_STUB_H
#define MACHINE_PROVIDER_STUB_H

#include "Interfaces/IMachineProvider.h"
#include "OTAServiceStub.h"
#include "BlockerStub.h"
#include "IBlocker.h"
#include "Logic/SensorDispatcher.h"

class MachineProviderStub : public ISensorProvider, public ISwitchProvider, public IThemeProvider {
public:
    MachineProviderStub(IBlocker* wifiSwitch, OTAServiceStub* ota, BlockerStub* warmingUp = nullptr) 
        : _wifiSwitch(wifiSwitch), _ota(ota), _warmingUp(warmingUp), _otaCreated(false) {}

    // IThemeProvider
    const std::vector<ITheme*>& getThemes() const override { return _themes; }

    // ISensorProvider
    ISensor* getBoilerPressure() override { return nullptr; }
    ISensor* getBoilerTemp() override { return nullptr; }
    ISensor* getShotTimer() override { return nullptr; }
    ISensorRegistry* getRegistry() override { return &_dispatcher; }

    // ISwitchProvider
    ISwitch* getPump() override { return nullptr; }
    IBlocker* getWiFiSwitch() override { return _wifiSwitch; }
    IBlocker* getOTASwitch() override { return _ota; }
    IBlocker* createOTA() override { 
        _otaCreated = true;
        return _ota; 
    }
    IBlocker* getWarmingUpBlocker() override { return _warmingUp; }
    ISwitch* getButtonRight() override { return nullptr; }
    ISwitch* getButtonLeft() override { return nullptr; }

    bool otaCreated() const { return _otaCreated; }

private:
    IBlocker* _wifiSwitch;
    OTAServiceStub* _ota;
    BlockerStub* _warmingUp;
    bool _otaCreated;
    std::vector<ITheme*> _themes;
    SensorDispatcher _dispatcher;
};

#endif
