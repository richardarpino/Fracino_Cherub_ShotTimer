#include "ScaleLogic.h"

ScaleLogic::ScaleLogic(ISwitch* pump, TaredWeight* weight, ISensorRegistry* registry)
    : _pump(pump), _weight(weight), _registry(registry) {}

void ScaleLogic::update() {
    if (!_pump || !_registry) return;

    _pump->update();

    if (_pump->justStarted()) {
        if (_weight) _weight->tare();
    }

    // Publish logical results to the Registry every frame
    if (_weight) {
        _registry->publish<TaredWeightTag>(_weight->getReading());
    }
}
