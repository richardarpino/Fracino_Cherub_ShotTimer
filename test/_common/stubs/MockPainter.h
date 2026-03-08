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
    class ISensorRegistry* lastRegistryPassed = nullptr;
    ScreenComposition lastComposition;

    void draw(const ScreenComposition& composition, class ISensorRegistry* registry) override {
        lastComposition = composition;
        lastRegistryPassed = registry;
        drawDashboardCallCount++; // Temporarily reuse for simple test compatibility if needed
    }

    void setLayout(uint8_t cols, uint8_t rows) override {
        setLayoutCallCount++;
    }
    
    void reset() {
        lastComposition = ScreenComposition();
        drawDashboardCallCount = 0;
        setLayoutCallCount = 0;
        lastRegistryPassed = nullptr;
    }
};

#endif
