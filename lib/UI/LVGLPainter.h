#ifndef LVGL_PAINTER_H
#define LVGL_PAINTER_H

#include "../Interfaces/IPainter.h"
#include "ScreenLayout.h"
#include "../Themes/ITheme.h"
#include "BlockerWidget.h"

// Concrete implementation of IPainter for LVGL
class LVGLPainter : public IPainter {
public:
    LVGLPainter();
    ~LVGLPainter();

    void init(lv_obj_t* parent, ITheme* initialTheme);
    void setTheme(ITheme* theme);
    
    // IPainter implementation
    void setLayout(uint8_t cols, uint8_t rows) override;
    void drawBlocker(const char* title, const char* message, float progress, bool isAlert) override;
    void drawGauge(const char* label, float value, float min, float max) override;
    void drawStatus(const char* label, const char* value, bool isAlert) override;
    void drawDashboard(class ISensorRegistry* registry) override;

    ScreenLayout* getLayout();

private:
    void ensureWidget(int slot, IWidget* (*factory)());

    enum class ActiveScreen { NONE, BLOCKER, DASHBOARD };
    ActiveScreen _activeScreen = ActiveScreen::NONE;

    ScreenLayout* _layout;
    lv_obj_t* _parent;
    ITheme* _theme;
    
    BlockerWidget* _blockerWidget;
};

#endif
