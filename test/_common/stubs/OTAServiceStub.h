#ifndef OTA_SERVICE_STUB_H
#define OTA_SERVICE_STUB_H

#include "../../lib/Interfaces/IOTAService.h"
#include <string>

class OTAServiceStub : public IOTAService {
public:
    void begin(const char* hostname) override { 
        _begun = true; 
        _hostname = hostname;
    }
    
    void update() override { _updateCalled = true; }
    
    Reading getReading() override { return { _progress, "%" }; }
    
    bool isActive() const override { return _active; }
    bool isReady() const override { return _begun; }
    bool isError() const override { return _error; }

    // Test Helpers
    bool begun() const { return _begun; }
    bool updateCalled() const { return _updateCalled; }
    void setActive(bool active) { _active = active; }
    void setProgress(float progress) { _progress = progress; }
    std::string getHostname() const { return _hostname; }

private:
    bool _begun = false;
    bool _updateCalled = false;
    bool _active = false;
    bool _error = false;
    float _progress = 0;
    std::string _hostname;
};

#endif
