#include "ShotScreen.h"

ShotScreen::ShotScreen(ISensorRegistry* registry) : _registry(registry) {
}

ShotScreen::~ShotScreen() {
}

void ShotScreen::update() {
    // No specific local update logic needed; relying on registry updates
}

bool ShotScreen::isDone() const {
    // Shot screen finishes when pump is no longer active.
    // We check the pump state from the registry.
    if (_registry) {
        Reading pump = _registry->getLatest<PumpReading>();
        return pump.value == 0.0f;
    }
    return false; // Safest default is to not spontaneously finish if reading is missing
}

void ShotScreen::paint(IPainter& p) {
    p.draw(getComposition(), _registry);
}
