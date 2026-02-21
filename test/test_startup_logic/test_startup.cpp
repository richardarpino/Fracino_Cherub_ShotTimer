#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "../_common/stubs/OTAServiceStub.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "Hardware/WiFiSensor.h"
#include "Hardware/WiFiSensor.cpp"
#include "Logic/StartupLogic.h"
#include "Logic/StartupLogic.cpp"

void test_startup_switch_behavior() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi;
    OTAServiceStub ota;
    StartupLogic logic(&wifi, &ota);

    TEST_ASSERT_FALSE(logic.isActive());
    TEST_ASSERT_FALSE(logic.justStarted());
    
    // Connect WiFi
    WiFi.setStatus(WL_CONNECTED);
    setMillis(100); wifi.update(); logic.update();
    TEST_ASSERT_FALSE(logic.isActive());

    // Hold wait (3s)
    setMillis(3100); wifi.update(); logic.update();
    TEST_ASSERT_FALSE(logic.isActive()); // Waiting for OTA Ready

    // OTA Ready -> READY state
    ota.setActive(true);
    logic.update();
    
    TEST_ASSERT_TRUE(logic.isActive());
    TEST_ASSERT_TRUE(logic.justStarted()); // First frame it became active
    
    // Next poll
    logic.update();
    TEST_ASSERT_TRUE(logic.isActive());
    TEST_ASSERT_FALSE(logic.justStarted()); // Should be consumed
}

void test_ota_not_started_without_wifi() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi;
    OTAServiceStub ota;
    StartupLogic logic(&wifi, &ota);

    logic.update();
    TEST_ASSERT_FALSE(ota.begun());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_startup_switch_behavior);
    RUN_TEST(test_ota_not_started_without_wifi);
    return UNITY_END();
}
