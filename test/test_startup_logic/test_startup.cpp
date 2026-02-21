#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "Hardware/WiFiSensor.h"
#include "Hardware/WiFiSensor.cpp"
#include "Logic/StartupLogic.h"
#include "Logic/StartupLogic.cpp"

void test_startup_transition() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi;
    StartupLogic logic(&wifi);

    TEST_ASSERT_FALSE(logic.isComplete());
    
    WiFi.setStatus(WL_CONNECTED);
    setMillis(100); wifi.update(); logic.update();
    TEST_ASSERT_FALSE(logic.isComplete()); // Still in hold period

    setMillis(3100); wifi.update(); logic.update();
    TEST_ASSERT_TRUE(logic.isComplete()); // Hold expired
}

void test_startup_just_finished() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi;
    StartupLogic logic(&wifi);

    TEST_ASSERT_FALSE(logic.justFinished());
    
    WiFi.setStatus(WL_CONNECTED);
    setMillis(100); wifi.update(); logic.update();
    TEST_ASSERT_FALSE(logic.justFinished()); // Still in hold

    setMillis(3100); wifi.update(); logic.update();
    TEST_ASSERT_TRUE(logic.justFinished());
    TEST_ASSERT_FALSE(logic.justFinished()); // Should clear after read
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_startup_transition);
    RUN_TEST(test_startup_just_finished);
    return UNITY_END();
}
