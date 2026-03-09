#ifndef LVGL_PAINTER_H
#define LVGL_PAINTER_H

#include "../Interfaces/IPainter.h"
#include "ScreenLayout.h"
#include "../Themes/ITheme.h"
#include "../Interfaces/IWidgetFactory.h"

// Concrete implementation of IPainter for LVGL
class LVGLPainter : public IPainter {
public:
    LVGLPainter();
    ~LVGLPainter();

    void init(lv_obj_t* parent, ITheme* initialTheme, IWidgetFactory* widgetFactory = nullptr);
    void setTheme(ITheme* theme);
    void setWidgetFactory(IWidgetFactory* factory) { _widgetFactory = factory; }
    
    // IPainter implementation
    void setLayout(uint8_t cols, uint8_t rows) override;
    void draw(const ScreenComposition& composition, class ISensorRegistry* registry) override;

    ScreenLayout* getLayout();

private:
    enum class ActiveScreen { NONE, BLOCKER, DASHBOARD, SHOT_TIMER };
    ActiveScreen _activeScreen = ActiveScreen::NONE;

    ScreenLayout* _layout;
    lv_obj_t* _parent;
    ITheme* _theme;
    IWidgetFactory* _widgetFactory;
    
    // Cache for current composition to prevent flickering
    uint32_t _compositionHash = 0;
    uint32_t calculateHash(const ScreenComposition& comp);
};

#endif
