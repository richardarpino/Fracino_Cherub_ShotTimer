#include <unity.h>
#include "../../lib/Logic/ThemeManager.h"
#include "../../lib/Logic/ThemeManager.cpp"
#include "../../lib/Themes/IThemeable.h"
#include "../../lib/Themes/ITheme.h"
#include "../../lib/Themes/DefaultTheme.h"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../_common/stubs/Arduino.h"

class ThemeableStub : public IThemeable {
public:
    void setTheme(ITheme* theme) override {
        _lastTheme = theme;
        _setCount++;
    }

    ITheme* lastTheme() const { return _lastTheme; }
    int setCount() const { return _setCount; }

private:
    ITheme* _lastTheme = nullptr;
    int _setCount = 0;
};

void test_theme_manager_initial_theme() {
    SensorDispatcher registry;
    ThemeableStub display;
    ThemeManager manager(&display, &registry);

    DefaultTheme theme;
    manager.addTheme(&theme);

    // After adding the first theme, it should be set on the display immediately
    TEST_ASSERT_EQUAL_PTR(&theme, display.lastTheme());
    TEST_ASSERT_EQUAL(1, display.setCount());
}

void test_theme_manager_switching() {
    SensorDispatcher registry;
    ThemeableStub display;
    ThemeManager manager(&display, &registry);

    DefaultTheme theme1;
    DefaultTheme theme2;
    manager.addTheme(&theme1);
    manager.addTheme(&theme2);

    // Initial state
    TEST_ASSERT_EQUAL_PTR(&theme1, display.lastTheme());

    // Press button (Simulate edge via Registry)
    registry.publish<ButtonRightTag>(Reading(1.0f, "", "ON", 0, false));
    registry.update();
    manager.update();

    TEST_ASSERT_EQUAL_PTR(&theme2, display.lastTheme());
    TEST_ASSERT_EQUAL(2, display.setCount());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_theme_manager_initial_theme);
    RUN_TEST(test_theme_manager_switching);
    return UNITY_END();
}
