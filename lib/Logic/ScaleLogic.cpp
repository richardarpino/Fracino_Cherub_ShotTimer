#include "ScaleLogic.h"

ScaleLogic::ScaleLogic(TaredWeight* weight, ISensorRegistry* registry)
    : _pump(registry), _weight(weight), _registry(registry) {}

void ScaleLogic::update() {
    if (!_registry) return;

    _pump.update();

    if (_pump.justStarted()) {
        if (_weight) _weight->tare();
    }

    // Publish logical results to the Registry every frame
    if (_weight) {
        _registry->publish<TaredWeightReading>(_weight->getReading());
    }
}
