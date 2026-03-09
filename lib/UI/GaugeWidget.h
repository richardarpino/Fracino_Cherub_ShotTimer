#ifndef GAUGE_WIDGET_H
#define GAUGE_WIDGET_H

#include <lvgl.h>
#include "IWidget.h"
#include "../Registry/ISensorRegistry.h"

/**
 * Base class for Gauge UI logic.
 */
class GaugeWidgetBase : public IWidget {
public:
    GaugeWidgetBase();
    virtual ~GaugeWidgetBase() = default;

    lv_obj_t* init(lv_obj_t* parent, uint8_t cols, uint8_t rows) override;
    void update(const Reading& reading) override;
    void applyTheme(ITheme* theme) override;
    
    // Explicit metadata update for scaling the meter
    void setMetadata(const SensorMetadata& meta) override;

protected:
    lv_obj_t* _container;
    lv_obj_t* _meter;
    lv_meter_scale_t* _scale;
    lv_meter_indicator_t* _indic;
    lv_obj_t* _unit_label;
    
    lv_color_t _bgColor, _textColor, _errorColor, _labelColor, _alertBgColor;
    
    uint16_t _tick_count;
    lv_coord_t _tick_len, _major_tick_len, _label_gap;

    static void meter_event_cb(lv_event_t* e);
};

/**
 * Templated Registry-Aware Gauge
 */
template<typename T = void>
class GaugeWidget : public GaugeWidgetBase {
public:
    GaugeWidget(ISensorRegistry* registry = nullptr) : _registry(registry) {}

    void setRegistry(ISensorRegistry* registry) override {
        _registry = registry;
    }

    void refresh() override {
        if (_registry) {
            refreshInternal(typename T::DataType{});
        }
    }

private:
    void refreshInternal(Reading) {
        setMetadata(_registry->getMetadata<T>());
        update(_registry->template getLatest<T>());
    }

    void refreshInternal(StatusMessage) {
        update(_registry->template getLatest<T>());
    }

private:
    ISensorRegistry* _registry;
};

/**
 * Late-binding specialization for string-based registration.
 */
template<>
class GaugeWidget<void> : public GaugeWidgetBase {
public:
    GaugeWidget(const char* tagName, ISensorRegistry* registry = nullptr) 
        : _tagName(tagName), _registry(registry) {}

    void setRegistry(ISensorRegistry* registry) override {
        _registry = registry;
    }

    void refresh() override {
        if (_registry) {
            Reading r = _registry->getLatestReading(_tagName.c_str());
            // Need a way to get metadata without type...
            // Let's add a virtual or bridging helper to ISensorRegistry.
            // For now, let's assume default gauge scaling or a generic lookup.
            update(r);
        }
    }

private:
    std::string _tagName;
    ISensorRegistry* _registry;
};
#endif
