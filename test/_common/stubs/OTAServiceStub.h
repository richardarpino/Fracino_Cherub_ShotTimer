#ifndef OTA_SERVICE_STUB_H
#define OTA_SERVICE_STUB_H

#include "../../lib/Interfaces/IBlocker.h"
#include <string>

class OTAServiceStub : public IBlocker {
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
    
    // IBlocker Implementation
    BlockerStatus getStatus() const override { 
        return BlockerStatus("OTA STUB", "OTA MSG", _progress, _failed); 
    }
    
    bool isActive() const override { return _active; }
    bool justStarted() const override { return _justStarted; }
    bool justStopped() const override { return _justStopped; }

    // Test Helpers
    bool begun() const { return _begun; }
    bool updateCalled() const { return _updateCalled; }
    void setActive(bool active) { _active = active; }
    void setProgress(float progress) { _progress = progress; }
    void setBegun(bool begun) { _begun = begun; }
    void setFailed(bool failed) { _failed = failed; }

private:
    bool _begun = false;
    bool _updateCalled = false;
    bool _active = false;
    bool _lastActive = false;
    bool _justStarted = false;
    bool _justStopped = false;
    bool _failed = false;
    float _progress = 0;
};

#endif
