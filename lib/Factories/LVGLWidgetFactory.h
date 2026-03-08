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

/**
 * Concrete factory for LVGL-based widgets.
 * Maps runtime tags to specific templated widget types.
 */
class LVGLWidgetFactory : public IWidgetFactory {
public:
    IWidget* createWidget(WidgetType type, const char* tagName, ISensorRegistry* registry) override {
        if (type == WidgetType::STATUS) {
            return new BlockerWidget(tagName);
        }

        if (type == WidgetType::GAUGE) {
            if (strcmp(tagName, BoilerPressureReading::NAME) == 0) return new GaugeWidget<BoilerPressureReading>(registry);
        }

        if (type == WidgetType::SENSOR) {
            if (strcmp(tagName, BoilerTempReading::NAME) == 0)   return new SensorWidget<BoilerTempReading>(registry);
            if (strcmp(tagName, ShotTimeReading::NAME) == 0)     return new SensorWidget<ShotTimeReading>(registry);
            if (strcmp(tagName, HeatingCycleReading::NAME) == 0) return new SensorWidget<HeatingCycleReading>(registry);
            if (strcmp(tagName, LastValidShotReading::NAME) == 0) return new SensorWidget<LastValidShotReading>(registry);
            if (strcmp(tagName, SystemUptimeReading::NAME) == 0) return new SensorWidget<SystemUptimeReading>(registry);
        }

        return nullptr;
    }
};

#endif
