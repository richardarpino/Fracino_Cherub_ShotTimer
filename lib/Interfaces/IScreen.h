#ifndef ISCREEN_H
#define ISCREEN_H

#include "../UI/ScreenLayout.h"

/**
 * Interface for a visual unit in a Workflow.
 * A Screen owns its Widgets and defines their layout.
 */
class IScreen {
public:
    virtual ~IScreen() {}
    virtual ScreenLayout* getLayout() = 0;
};

#endif
