#ifndef ITHEMEABLE_H
#define ITHEMEABLE_H

#include "ITheme.h"

/**
 * Interface for objects that can have a visual theme applied.
 * Allows decoupling logic coordinators from concrete display classes.
 */
class IThemeable {
public:
    virtual ~IThemeable() = default;
    virtual void setTheme(ITheme* theme) = 0;
};

#endif
