#include "ScaleLogic.h"

ScaleLogic::ScaleLogic(ISwitch* pump, ManualPumpTimer* timer, TaredWeight* weight, 
                       BoilerTemperature* temp, ISensorRegistry* registry)
    : _pump(pump), _timer(timer), _weight(weight), _temp(temp), _registry(registry) {}

void ScaleLogic::update() {
    if (!_pump) return;

    // 1. External Drive: Poll the hardware switch
    _pump->update();

    // 2. Orchestration: Capture edges and drive logical components
    if (_pump->justStarted()) {
        if (_timer) _timer->start();
        if (_weight) _weight->tare();
    }

    if (_pump->justStopped()) {
        if (_timer) _timer->stop();
    }

    // 3. Integration: Publish logical results to the Registry every frame
    if (_registry) {
        if (_timer) _registry->publish<ShotTimeTag>(_timer->getReading());
        if (_weight) _registry->publish<TaredWeightTag>(_weight->getReading());
        if (_temp) _registry->publish<BoilerTempTag>(_temp->getReading());
    }
}
