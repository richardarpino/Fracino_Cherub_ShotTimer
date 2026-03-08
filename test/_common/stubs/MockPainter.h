#ifndef MOCK_PAINTER_H
#define MOCK_PAINTER_H

#include "../../lib/Interfaces/IPainter.h"
#include <string>

class MockPainter : public IPainter {
public:
    int drawGaugeCallCount = 0;
    int drawStatusCallCount = 0;
    int drawBlockerCallCount = 0;
    int setLayoutCallCount = 0;
    int drawDashboardCallCount = 0;
    int drawShotTimerCallCount = 0;

    std::string lastBlockerTitle = "";
    std::string lastBlockerMessage = "";
    float lastBlockerProgress = -1.0f;
    bool lastBlockerAlert = false;
    class ISensorRegistry* lastRegistryPassed = nullptr;

    void drawBlocker(const char* title, const char* message, float progress, bool isAlert) override {
        drawBlockerCallCount++;
        lastBlockerTitle = title ? title : "";
        lastBlockerMessage = message ? message : "";
        lastBlockerProgress = progress;
        lastBlockerAlert = isAlert;
    }

    void drawGauge(const char* label, float value, float min, float max) override {
        drawGaugeCallCount++;
    }

    void drawStatus(const char* label, const char* value, bool isAlert) override {
        drawStatusCallCount++;
    }

    void drawDashboard(class ISensorRegistry* registry) override {
        drawDashboardCallCount++;
        lastRegistryPassed = registry;
    }

    void drawShotTimer(class ISensorRegistry* registry) override {
        drawShotTimerCallCount++;
        lastRegistryPassed = registry;
    }

    void setLayout(uint8_t cols, uint8_t rows) override {
        setLayoutCallCount++;
    }
    
    void reset() {
        drawGaugeCallCount = 0;
        drawStatusCallCount = 0;
        drawDashboardCallCount = 0;
        drawBlockerCallCount = 0;
        setLayoutCallCount = 0;
        lastBlockerTitle = "";
        lastBlockerMessage = "";
        lastBlockerProgress = -1.0f;
        lastBlockerAlert = false;
        lastRegistryPassed = nullptr;
    }
};

#endif
