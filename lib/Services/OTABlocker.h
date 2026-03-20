#ifndef OTA_BLOCKER_H
#define OTA_BLOCKER_H

#include "../Interfaces/IBlocker.h"
#include "../Registry/ISensorRegistry.h"
#include "../Interfaces/SensorTags.h"

class OTABlocker : public IBlocker {
public:
    OTABlocker(ISensorRegistry* registry);
    virtual ~OTABlocker() = default;
    
    // IBlocker Implementation
    StatusMessage getStatus() const override;
    const char* getTagName() const override { return OTAStatus::NAME; }
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
