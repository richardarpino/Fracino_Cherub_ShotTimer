#ifndef MACHINE_PROVIDER_STUB_H
#define MACHINE_PROVIDER_STUB_H

#include "Interfaces/IMachineProvider.h"
#include "OTAServiceStub.h"

class MachineProviderStub : public ISensorProvider, public ISwitchProvider, public IThemeProvider {
public:
    MachineProviderStub(ISwitch* wifiSwitch, ISensor* wifiSensor, OTAServiceStub* ota) 
        : _wifiSwitch(wifiSwitch), _wifiSensor(wifiSensor), _ota(ota), _otaCreated(false) {}

    // IThemeProvider
    const std::vector<ITheme*>& getThemes() const override { return _themes; }

    // ISensorProvider
    ISensor* getBoilerPressure() override { return nullptr; }
    ISensor* getBoilerTemp() override { return nullptr; }
    ISensor* getShotTimer() override { return nullptr; }
    ISensor* getOTASensor() override { return _ota; }
    ISensor* getWiFiSensor() override { return _wifiSensor; }

    // ISwitchProvider
    ISwitch* getPump() override { return nullptr; }
    ISwitch* getWiFiSwitch() override { return _wifiSwitch; }
    ISwitch* getOTASwitch() override { return _ota; }
    ISwitch* createOTA() override { 
        _otaCreated = true;
        return _ota; 
    }

    bool otaCreated() const { return _otaCreated; }

private:
    ISwitch* _wifiSwitch;
    ISensor* _wifiSensor;
    OTAServiceStub* _ota;
    bool _otaCreated;
    std::vector<ITheme*> _themes;
};

#endif
