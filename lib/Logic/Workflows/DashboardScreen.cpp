#include "DashboardScreen.h"

DashboardScreen::DashboardScreen(ISensorRegistry* registry) : _registry(registry) {
}

DashboardScreen::~DashboardScreen() {
}

void DashboardScreen::paint(IPainter& p) {
    p.draw(getComposition(), _registry);
}
