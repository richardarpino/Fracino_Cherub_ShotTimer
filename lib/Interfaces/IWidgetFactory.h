#ifndef IWIDGET_FACTORY_H
#define IWIDGET_FACTORY_H

#include "IWidget.h"
#include "SensorTypes.h"

/**
 * Interface for creating UI widgets.
 * Standardizes how widgets are instantiated from metadata tags.
 */
class IWidgetFactory {
public:
    virtual ~IWidgetFactory() {}

    /**
     * Creates a widget of the specified type bound to a tag.
     */
    virtual IWidget* createWidget(WidgetType type, const char* tagName, class ISensorRegistry* registry) = 0;
};

#endif
