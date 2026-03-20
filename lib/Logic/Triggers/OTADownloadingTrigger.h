#ifndef OTA_DOWNLOADING_TRIGGER_H
#define OTA_DOWNLOADING_TRIGGER_H

#include "../../Interfaces/ITrigger.h"
#include "../../Registry/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"

class OTADownloadingTrigger : public ITrigger {
public:
    OTADownloadingTrigger(ISensorRegistry* registry) : _registry(registry) {}
    
    void update() override {}
    
    bool isActive() const override {
        if (!_registry) return false;
        StatusMessage status = _registry->getLatest<OTAStatus>();
        
        // Active during download: 0 <= progress < 100, and not in an error state.
        // Note: 100% means it's either finished or just listening.
        return (status.progress >= 0.0f && status.progress < 100.0f && !status.isFailed); 
    }

private:
    ISensorRegistry* _registry;
};

#endif
