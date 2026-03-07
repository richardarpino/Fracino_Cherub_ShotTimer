#include "DashboardScreen.h"

 DashboardScreen::DashboardScreen(ISensorRegistry* registry) {
    _layout = new ScreenLayout();
    _layout->setRegistry(registry);

    _layout->addWidget(new SensorWidget<BoilerPressureReading>(registry));
    _layout->addWidget(new SensorWidget<BoilerTempReading>(registry));
    _layout->addWidget(new SensorWidget<HeatingCycleReading>(registry));
    _layout->addWidget(new SensorWidget<ShotTimeReading>(registry));
}

DashboardScreen::~DashboardScreen() {
    delete _layout;
}

ScreenLayout* DashboardScreen::getLayout() {
    return _layout;
}

void DashboardScreen::update() {
    // Registry-aware widgets handle their own refresh
    _layout->update();
}
