#ifndef BLOCKER_STUB_H
#define BLOCKER_STUB_H

#include "../../lib/Interfaces/IBlocker.h"

class BlockerStub : public IBlocker {
public:
    void update() override { 
        _justStarted = _active && !_lastActive;
        _justStopped = !_active && _lastActive;
        _lastActive = _active;
        _updateCalled = true; 
    }
    
    // IBlocker Implementation
    BlockerStatus getStatus() const override { 
        return BlockerStatus(_title, _message, _progress, _failed); 
    }
    
    bool isActive() const override { return _active; }
    bool justStarted() const override { return _justStarted; }
    bool justStopped() const override { return _justStopped; }

    void setActive(bool active) { _active = active; }
    void setTitle(String title) { _title = title; }
    void setMessage(String message) { _message = message; }
    void setProgress(float progress) { _progress = progress; }
    void setFailed(bool failed) { _failed = failed; }
    bool updateCalled() const { return _updateCalled; }

private:
    bool _updateCalled = false;
    bool _active = false;
    bool _lastActive = false;
    bool _justStarted = false;
    bool _justStopped = false;
    String _title = "STUB";
    String _message = "STUB MSG";
    bool _failed = false;
    float _progress = 0;
};

#endif
