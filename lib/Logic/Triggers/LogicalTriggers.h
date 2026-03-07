#ifndef LOGICAL_TRIGGERS_H
#define LOGICAL_TRIGGERS_H

#include "../../Interfaces/ISwitch.h"

/**
 * Logical AND of two triggers.
 */
class AndTrigger : public ISwitch {
public:
    AndTrigger(ISwitch* a, ISwitch* b) : _a(a), _b(b), _isActive(false), _lastActive(false) {}

    void update() override {
        if (!_a || !_b) return;
        _a->update();
        _b->update();
        _lastActive = _isActive;
        _isActive = _a->isActive() && _b->isActive();
    }

    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _isActive && !_lastActive; }
    bool justStopped() const override { return !_isActive && _lastActive; }

private:
    ISwitch* _a;
    ISwitch* _b;
    bool _isActive;
    bool _lastActive;
};

/**
 * Logical OR of two triggers.
 */
class OrTrigger : public ISwitch {
public:
    OrTrigger(ISwitch* a, ISwitch* b) : _a(a), _b(b), _isActive(false), _lastActive(false) {}

    void update() override {
        if (!_a || !_b) return;
        _a->update();
        _b->update();
        _lastActive = _isActive;
        _isActive = _a->isActive() || _b->isActive();
    }

    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _isActive && !_lastActive; }
    bool justStopped() const override { return !_isActive && _lastActive; }

private:
    ISwitch* _a;
    ISwitch* _b;
    bool _isActive;
    bool _lastActive;
};

/**
 * Logical NOT of a trigger.
 */
class NotTrigger : public ISwitch {
public:
    NotTrigger(ISwitch* a) : _a(a), _isActive(false), _lastActive(false) {}

    void update() override {
        if (!_a) return;
        _a->update();
        _lastActive = _isActive;
        _isActive = !_a->isActive();
    }

    bool isActive() const override { return _isActive; }
    bool justStarted() const override { return _isActive && !_lastActive; }
    bool justStopped() const override { return !_isActive && _lastActive; }

private:
    ISwitch* _a;
    bool _isActive;
    bool _lastActive;
};

#endif
