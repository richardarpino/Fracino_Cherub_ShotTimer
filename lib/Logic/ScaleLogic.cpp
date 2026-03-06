#include "ScaleLogic.h"

ScaleLogic::ScaleLogic(TaredWeightProcessor* weight, ISensorRegistry* registry)
    : _pump(registry), _weight(weight), _registry(registry) {}

void ScaleLogic::update() {
    if (!_registry) return;

    // Use the weight processor's autonomous update
    if (_weight) {
        _weight->update();
    }
}
