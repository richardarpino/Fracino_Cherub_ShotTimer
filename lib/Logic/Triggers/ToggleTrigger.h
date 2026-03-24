#ifndef TOGGLE_TRIGGER_H
#define TOGGLE_TRIGGER_H

#include "../../Interfaces/ITrigger.h"
#include "../../Sensors/Registry/RegistrySwitch.h"
#include "../../Interfaces/SensorTags.h"

/**
 * A latched trigger that toggles its state on each button press.
 * SRP: Manages a boolean state based on discrete events.
 */
class ToggleTrigger : public ITrigger {
public:
    ToggleTrigger(ISwitch* button) : _button(button), _active(false) {}

    void update() override {
        if (!_button) return;
        
        _button->update();
        if (_button->justStarted()) {
            _active = !_active;
        }
    }

    bool isActive() const override {
        return _active;
    }

private:
    ISwitch* _button;
    bool _active;
};

#endif
