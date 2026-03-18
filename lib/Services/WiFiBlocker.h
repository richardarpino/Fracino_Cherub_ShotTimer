#ifndef WIFI_BLOCKER_H
#define WIFI_BLOCKER_H

#include "../Interfaces/IBlocker.h"
#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"

class WiFiBlocker : public IBlocker {
public:
    WiFiBlocker(ISensorRegistry* registry);
    virtual ~WiFiBlocker() = default;
    
    // IBlocker Implementation
    StatusMessage getStatus() const override;
    const char* getTagName() const override { return WiFiStatus::NAME; }
    void update() override;

    // ISwitch Implementation
    bool isActive() const override;
    bool justStarted() const override;
    bool justStopped() const override;

private:
    ISensorRegistry* _registry;
    bool _isActive;
    bool _wasActive;
};

#endif
