#include <unity.h>
#include "MachineFactory.h"
#include "MachineFactory.cpp"
#include "ThemeManager.h"
#include "ThemeManager.cpp"
#include "../../test/_common/stubs/SensorStub.h"
#include "../../test/_common/stubs/BlockerStub.h"
#include "../../test/_common/stubs/Arduino.cpp"
#include "../../test/_common/stubs/WiFi.cpp"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"

// Mock Display
class MockDisplay : public IThemeable {
public:
    ITheme* currentTheme = nullptr;
    void setTheme(ITheme* theme) override { currentTheme = theme; }
};

// Mock Theme
class MockTheme : public ITheme {
public:
    uint16_t getBackgroundColor() override { return 0; }
    uint16_t getTextColor() override { return 0; }
    uint16_t getLabelColor() override { return 0; }
    uint16_t getAlertTextColor() override { return 0; }
    uint16_t getAlertBackgroundColor() override { return 0; }
};

void test_theme_manager_uses_registry_edge() {
    SensorDispatcher registry;
    MockDisplay display;
    ThemeManager manager(&display, &registry);

    MockTheme theme1, theme2;
    manager.addTheme(&theme1);
    manager.addTheme(&theme2);

    // Initial state
    TEST_ASSERT_EQUAL_PTR(&theme1, display.currentTheme);

    // Simulate press (edge via registry)
    registry.publish<ButtonRightReading>(1.0f);
    registry.update();
    manager.update();
    TEST_ASSERT_EQUAL_PTR(&theme2, display.currentTheme);

    // Simulate held (no change in registry state)
    registry.update();
    manager.update();
    TEST_ASSERT_EQUAL_PTR(&theme2, display.currentTheme);
}

void test_factory_provides_dual_buttons() {
    MachineConfig config = {"host", "ssid", "pass", 50};
    MachineFactory factory(config);
    
    ISwitchProvider* provider = &factory;
    
    ISwitch* right = provider->getButtonRight();
    ISwitch* left = provider->getButtonLeft();
    
    TEST_ASSERT_NOT_NULL(right);
    TEST_ASSERT_NOT_NULL(left);
    TEST_ASSERT_NOT_EQUAL(right, left);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_theme_manager_uses_registry_edge);
    RUN_TEST(test_factory_provides_dual_buttons);
    return UNITY_END();
}
