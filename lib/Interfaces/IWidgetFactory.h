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
     * Creates a widget by its registered name bound to a tag.
     */
    virtual IWidget* createWidget(const char* widgetName, const char* tagName, class ISensorRegistry* registry) = 0;
};

#endif
