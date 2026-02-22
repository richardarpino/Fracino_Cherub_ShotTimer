#ifndef OTA_SERVICE_STUB_H
#define OTA_SERVICE_STUB_H

#include "../../lib/Interfaces/ISensor.h"
#include "../../lib/Interfaces/ISwitch.h"
#include <string>

class OTAServiceStub : public ISensor, public ISwitch {
public:
    void begin() { 
        _begun = true; 
    }
    
    void update() override { 
        _justStarted = _active && !_lastActive;
        _justStopped = !_active && _lastActive;
        _lastActive = _active;
        _updateCalled = true; 
    }
    
    Reading getReading() override { return { _progress, "%" }; }
    
    bool isActive() const override { return _active; }
    bool justStarted() const override { return _justStarted; }
    bool justStopped() const override { return _justStopped; }

    // Test Helpers
    bool begun() const { return _begun; }
    bool updateCalled() const { return _updateCalled; }
    void setActive(bool active) { _active = active; }
    void setProgress(float progress) { _progress = progress; }
    void setBegun(bool begun) { _begun = begun; }

private:
    bool _begun = false;
    bool _updateCalled = false;
    bool _active = false;
    bool _lastActive = false;
    bool _justStarted = false;
    bool _justStopped = false;
    float _progress = 0;
};

#endif
