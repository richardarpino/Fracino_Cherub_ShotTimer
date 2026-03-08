#include "LVGLPainter.h"
#include "SensorWidget.h"
#include "../Interfaces/SensorTags.h"

LVGLPainter::LVGLPainter() : _layout(nullptr), _parent(nullptr), _theme(nullptr), _blockerWidget(nullptr) {
    _layout = new ScreenLayout();
}

LVGLPainter::~LVGLPainter() {
    delete _layout;
    if (_blockerWidget) delete _blockerWidget;
}

void LVGLPainter::init(lv_obj_t* parent, ITheme* initialTheme) {
    _parent = parent;
    _theme = initialTheme;
    _layout->init(parent);
    if (_theme) {
        _layout->applyTheme(_theme);
    }
}

void LVGLPainter::setTheme(ITheme* theme) {
    _theme = theme;
    if (_layout) {
        _layout->applyTheme(theme);
    }
}

ScreenLayout* LVGLPainter::getLayout() {
    return _layout;
}

void LVGLPainter::setLayout(uint8_t cols, uint8_t rows) {
    _layout->setDimensions(cols, rows);
    // When layout changes, we might want to clear existing widgets logically,
    // but ScreenLayout handles its internal grid.
}

void LVGLPainter::drawBlocker(const char* title, const char* message, float progress, bool isAlert) {
    if (_activeScreen != ActiveScreen::BLOCKER) {
        _layout->reset();
        _layout->setDimensions(1, 1);
        _blockerWidget = new BlockerWidget(nullptr);
        _layout->addWidget(_blockerWidget);
        if (_parent) {
            _layout->init(_parent);
            if (_theme) _layout->applyTheme(_theme);
        }
        _activeScreen = ActiveScreen::BLOCKER;
    }
    
    StatusMessage statusMsg;
    statusMsg.title = title ? title : "";
    statusMsg.message = message ? message : "";
    statusMsg.isFailed = isAlert;
    statusMsg.progress = progress;
    
    _blockerWidget->setStatus(statusMsg);
    _blockerWidget->refresh();
}

void LVGLPainter::drawGauge(const char* label, float value, float min, float max) {
    // Unused directly; SensorWidget handles gauge drawing internally
}

void LVGLPainter::drawStatus(const char* label, const char* value, bool isAlert) {
    // Unused directly
}

void LVGLPainter::drawDashboard(ISensorRegistry* registry) {
    if (_activeScreen != ActiveScreen::DASHBOARD) {
        _layout->reset();
        _layout->setDimensions(2, 2);
        _layout->addWidget(new SensorWidget<BoilerPressureReading>(registry));
        _layout->addWidget(new SensorWidget<BoilerTempReading>(registry));
        _layout->addWidget(new SensorWidget<HeatingCycleReading>(registry));
        _layout->addWidget(new SensorWidget<LastValidShotReading>(registry));
        if (_parent) {
            _layout->init(_parent);
            if (_theme) _layout->applyTheme(_theme);
        }
        _blockerWidget = nullptr; // Was deleted by reset()
        _activeScreen = ActiveScreen::DASHBOARD;
    }
    
    _layout->update();
}

void LVGLPainter::drawShotTimer(ISensorRegistry* registry) {
    if (_activeScreen != ActiveScreen::SHOT_TIMER) {
        _layout->reset();
        _layout->setDimensions(2, 1);
        _layout->addWidget(new SensorWidget<LastValidShotReading>(registry));
        _layout->addWidget(new SensorWidget<ShotTimeReading>(registry));
        if (_parent) {
            _layout->init(_parent);
            if (_theme) _layout->applyTheme(_theme);
        }
        _blockerWidget = nullptr;
        _activeScreen = ActiveScreen::SHOT_TIMER;
    }
    
    _layout->update();
}
