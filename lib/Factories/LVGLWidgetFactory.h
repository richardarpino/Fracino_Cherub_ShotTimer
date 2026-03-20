#ifndef LVGL_WIDGET_FACTORY_H
#define LVGL_WIDGET_FACTORY_H

#include "../Interfaces/IWidgetFactory.h"
#include "../UI/GaugeWidget.h"
#include "../UI/SensorWidget.h"
#include "../UI/BlockerWidget.h"

/**
 * Concrete factory for LVGL-based widgets.
 */
#include "../Interfaces/SensorTags.h"

#include "../Registry/IWidgetRegistry.h"
#include "../Registry/WidgetTags.h"

#include <map>
#include <string>
#include <functional>

/**
 * Concrete factory for LVGL-based widgets.
 * Maps runtime tags to specific templated widget types via late-binding creators.
 */
class LVGLWidgetFactory : public IWidgetFactory {
public:
    using WidgetCreator = std::function<IWidget*(const char* tagName, ISensorRegistry* registry)>;

    LVGLWidgetFactory(IWidgetRegistry* registry = nullptr) : _registry(registry) {}

    void setRegistry(IWidgetRegistry* registry) {
        _registry = registry;
    }

    void registerCreator(const char* widgetName, WidgetCreator creator) {
        _creators[widgetName] = creator;
    }

    static void registerStandardCreators(LVGLWidgetFactory& factory) {
        factory.registerCreator(SensorWidgetTag::NAME, [](const char* tag, ISensorRegistry* reg) { return new SensorWidget<void>(tag, reg); });
        factory.registerCreator(GaugeWidgetTag::NAME, [](const char* tag, ISensorRegistry* reg) { return new GaugeWidget<void>(tag, reg); });
        factory.registerCreator(BlockerWidgetTag::NAME, [](const char* tag, ISensorRegistry* reg) { return new BlockerWidget(tag); });
        factory.registerCreator(ShotTimerWidgetTag::NAME, [](const char* tag, ISensorRegistry* reg) { return new SensorWidget<void>(tag, reg); });
    }

    IWidget* createWidget(const char* widgetName, const char* tagName, ISensorRegistry* sensorRegistry) override {
        // 1. Compatibility Check (Audit layer)
        if (_registry && !_registry->isCompatible(widgetName, tagName)) {
            return nullptr;
        }

        // 2. Late-binding lookup
        auto it = _creators.find(widgetName);
        if (it != _creators.end()) {
            return it->second(tagName, sensorRegistry);
        }

        return nullptr;
    }

private:
    IWidgetRegistry* _registry;
    std::map<std::string, WidgetCreator> _creators;
};

#endif
