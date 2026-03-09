#ifndef IWIDGET_REGISTRY_H
#define IWIDGET_REGISTRY_H

#include "../Interfaces/SensorTypes.h"
#include <vector>

struct WidgetCompatibility {
    DataCategory category;
    std::vector<PhysicalQuantity> quantities;
    std::vector<const char*> specificTags;
};

class IWidgetRegistry {
public:
    virtual ~IWidgetRegistry() {}
    virtual bool isCompatible(const char* widgetName, const char* tagName) = 0;
};

#endif
