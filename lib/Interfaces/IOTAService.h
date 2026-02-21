#ifndef IOTA_SERVICE_H
#define IOTA_SERVICE_H

#include "ISensor.h"

class IOTAService : public ISensor {
public:
    virtual ~IOTAService() = default;
    
    virtual void begin(const char* hostname) = 0;
    virtual void update() = 0;
    
    virtual bool isActive() const = 0;
    virtual bool isReady() const = 0;
    virtual bool isError() const = 0;
};

#endif
