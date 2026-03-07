#ifndef BLOCKER_WIDGET_H
#define BLOCKER_WIDGET_H

#include "IWidget.h"
#include "../Interfaces/IBlocker.h"

class BlockerWidget : public IWidget {
public:
    BlockerWidget(IBlocker* blocker);
    
    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override;
    void update(const Reading& reading) override;
    void refresh() override;
    void applyTheme(ITheme* theme) override;
    
    // Explicitly push status when used by a Painter
    void setStatus(const StatusMessage& status);

private:
    lv_obj_t* _container;
    lv_obj_t* _title_label;
    lv_obj_t* _status_label;
    lv_obj_t* _bar;
    IBlocker* _blocker;
    
    StatusMessage _lastStatus;
    
    lv_color_t _bgColor;
    lv_color_t _textColor;
    lv_color_t _labelColor;
    lv_color_t _errorColor;
    lv_color_t _alertBgColor;
};

#endif
