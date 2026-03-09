#ifndef IWIDGET_REGISTRY_H
#define IWIDGET_REGISTRY_H

#include "../Interfaces/SensorTypes.h"
#include <vector>

struct WidgetCompatibility {
    DataCategory category = DataCategory::TELEMETRY;
    std::vector<PhysicalQuantity> quantities;
    std::vector<const char*> specificTags;

    WidgetCompatibility() = default;
    WidgetCompatibility(DataCategory cat, 
                        std::vector<PhysicalQuantity> q = {}, 
                        std::vector<const char*> tags = {})
        : category(cat), quantities(q), specificTags(tags) {}
};

class IWidgetRegistry {
public:
    virtual ~IWidgetRegistry() {}
    virtual bool isCompatible(const char* widgetName, const char* tagName) = 0;
};

#endif
