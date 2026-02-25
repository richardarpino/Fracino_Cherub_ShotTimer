#include <unity.h>
#include "../../lib/Logic/ThemeManager.h"
#include "../../lib/Themes/IThemeable.h"
#include "../../lib/Themes/DefaultTheme.h"
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

class MockSwitch : public ISwitch {
public:
    void update() override {}
    bool isActive() const override { return _active; }
    bool justStarted() const override { return _started; }
    bool justStopped() const override { return false; }
    
    void setActive(bool active) { _active = active; }
    void setStarted(bool started) { _started = started; }

private:
    bool _active = false;
    bool _started = false;
};

void test_theme_manager_initial_theme() {
    ThemeableStub display;
    MockSwitch button;
    ThemeManager manager(&display, &button);

    DefaultTheme theme;
    manager.addTheme(&theme);

    // After adding the first theme, it should be set on the display immediately
    TEST_ASSERT_EQUAL_PTR(&theme, display.lastTheme());
    TEST_ASSERT_EQUAL(1, display.setCount());
}

void test_theme_manager_switching() {
    ThemeableStub display;
    MockSwitch button;
    ThemeManager manager(&display, &button);

    DefaultTheme theme1;
    DefaultTheme theme2;
    manager.addTheme(&theme1);
    manager.addTheme(&theme2);

    // Initial state
    TEST_ASSERT_EQUAL_PTR(&theme1, display.lastTheme());

    // Press button (Simulate edge)
    button.setStarted(true);
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
