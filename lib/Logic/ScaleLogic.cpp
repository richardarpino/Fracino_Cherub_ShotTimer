#include "ScaleLogic.h"

ScaleLogic::ScaleLogic(ISwitch* pump, ShotTimer* timer, TaredWeight* weight)
    : _pump(pump), _timer(timer), _weight(weight) {}

void ScaleLogic::update() {
    if (!_pump) return;

    if (_pump->justStarted()) {
        if (_timer) _timer->start();
        if (_weight) _weight->tare();
    }

    if (_pump->justStopped()) {
        if (_timer) _timer->stop();
    }
}
