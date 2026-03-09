#ifndef WIDGET_REGISTRY_H
#define WIDGET_REGISTRY_H

#include "IWidgetRegistry.h"
#include "ISensorRegistry.h"

#include "WidgetRegistry.h"
#include <algorithm>
#include <map>
#include <string>

class WidgetRegistry : public IWidgetRegistry {
public:
    WidgetRegistry(ISensorRegistry* sensorRegistry) : _sensorRegistry(sensorRegistry) {}

    template<typename T>
    void registerWidget(WidgetCompatibility compatibility) {
        _widgets[T::NAME] = compatibility;
    }

    bool isCompatible(const char* widgetName, const char* tagName) override {
        if (_widgets.find(widgetName) == _widgets.end()) {
            return false;
        }

        const auto& comp = _widgets[widgetName];

        // Step 1: Specific Match (Higher Precedence)
        for (const char* specificTag : comp.specificTags) {
            if (strcmp(specificTag, tagName) == 0) return true;
        }

        // Step 2: Categorical Match
        DataCategory tagCategory = _sensorRegistry->getCategory(tagName);
        if (tagCategory != comp.category) return false;

        if (tagCategory == DataCategory::SERVICE) {
            return true; // Generic service support
        }

        // Telemetry: Match by PhysicalQuantity
        PhysicalQuantity tagQuantity = _sensorRegistry->getQuantity(tagName);
        for (auto q : comp.quantities) {
            if (q == tagQuantity) return true;
        }

        return false;
    }

private:
    ISensorRegistry* _sensorRegistry;
    std::map<std::string, WidgetCompatibility> _widgets;
};

#endif
