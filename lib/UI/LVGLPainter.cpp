#include "LVGLPainter.h"
#include "SensorWidget.h"
#include "GaugeWidget.h"
#include "StatusWidget.h"
#include "BlockerWidget.h"
#include "../Interfaces/SensorTags.h"
#include <cstring>
#include <cstdint>

LVGLPainter::LVGLPainter() 
    : _layout(new ScreenLayout()), _parent(nullptr), _theme(nullptr), _widgetFactory(nullptr) {}

LVGLPainter::~LVGLPainter() {
    delete _layout;
}

void LVGLPainter::init(lv_obj_t* parent, ITheme* initialTheme, IWidgetFactory* widgetFactory) {
    _parent = parent;
    _theme = initialTheme;
    _widgetFactory = widgetFactory;
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
}

// Replaced by IWidgetFactory

uint32_t LVGLPainter::calculateHash(const ScreenComposition& comp) {
    uint32_t h = comp.cols ^ (comp.rows << 8);
    for (const auto& w : comp.widgets) {
        h ^= (uint32_t)w.type;
        // Simple string hash
        if (w.tag) {
            for (const char* p = w.tag; *p; p++) h = (h * 31) + *p;
        }
    }
    return h;
}

void LVGLPainter::draw(const ScreenComposition& composition, ISensorRegistry* registry) {
    _layout->setRegistry(registry);
    uint32_t newHash = calculateHash(composition);
    
    // Only rebuild if the screen structure changed
    if (newHash != _compositionHash) {
        _layout->reset();
        _layout->setDimensions(composition.cols, composition.rows);
        
        for (const auto& w : composition.widgets) {
            if (_widgetFactory) {
                IWidget* widget = _widgetFactory->createWidget(w.type, w.tag, registry);
                if (widget) {
                    _layout->addWidget(widget);
                }
            }
        }
        
        if (_parent) {
            _layout->init(_parent);
            if (_theme) _layout->applyTheme(_theme);
        }
        
        _compositionHash = newHash;
    }
    
    // Refresh data in all existing widgets
    _layout->update();
}
