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

/**
 * Concrete factory for LVGL-based widgets.
 * Maps runtime tags to specific templated widget types.
 */
class LVGLWidgetFactory : public IWidgetFactory {
public:
    LVGLWidgetFactory(IWidgetRegistry* registry = nullptr) : _registry(registry) {}

    void setRegistry(IWidgetRegistry* registry) {
        _registry = registry;
    }

    IWidget* createWidget(const char* widgetName, const char* tagName, ISensorRegistry* sensorRegistry) override {
        // 1. Compatibility Check
        if (_registry && !_registry->isCompatible(widgetName, tagName)) {
            return nullptr;
        }

        // 2. Late-binding instantiation
        if (strcmp(widgetName, BlockerWidgetTag::NAME) == 0) {
            return new BlockerWidget(tagName);
        }

        if (strcmp(widgetName, GaugeWidgetTag::NAME) == 0) {
            return new GaugeWidget<void>(tagName, sensorRegistry);
        }

        if (strcmp(widgetName, SensorWidgetTag::NAME) == 0) {
            return new SensorWidget<void>(tagName, sensorRegistry);
        }

        // Default or specialized overrides (like ShotTimer)
        if (strcmp(widgetName, ShotTimerWidgetTag::NAME) == 0) {
            return new SensorWidget<void>(tagName, sensorRegistry); // ShotTimer currently uses SensorWidget base logic
        }

        return nullptr;
    }

private:
    IWidgetRegistry* _registry;
};

#endif
