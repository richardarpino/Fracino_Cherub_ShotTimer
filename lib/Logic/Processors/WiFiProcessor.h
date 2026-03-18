#ifndef WIFI_PROCESSOR_H
#define WIFI_PROCESSOR_H

#include "../../Interfaces/ITagProcessor.h"
#include "../../Registry/ISensorRegistry.h"
#include "../../Interfaces/SensorTags.h"

class WiFiProcessor : public ITagProcessor {
public:
    WiFiProcessor(ISensorRegistry* registry);
    void update() override;

private:
    ISensorRegistry* _registry;
    mutable char _statusBuffer[64];
};

#endif
