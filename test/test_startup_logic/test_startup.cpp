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

void test_startup_transition() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi;
    OTAServiceStub ota;
    StartupLogic logic(&wifi, &ota);

    TEST_ASSERT_EQUAL(StartupLogic::State::SEARCHING_WIFI, logic.getState());
    
    // WiFi Connected but still in hold
    WiFi.setStatus(WL_CONNECTED);
    setMillis(100); wifi.update(); logic.update();
    TEST_ASSERT_EQUAL(StartupLogic::State::WIFI_STABLE, logic.getState());
    TEST_ASSERT_FALSE(ota.begun());

    // WiFi Hold Expired
    setMillis(3100); wifi.update(); logic.update();
    TEST_ASSERT_EQUAL(StartupLogic::State::OTA_STARTING, logic.getState());
    TEST_ASSERT_TRUE(ota.begun());

    // OTA Active
    ota.setActive(true);
    logic.update();
    TEST_ASSERT_EQUAL(StartupLogic::State::READY, logic.getState());
}

void test_ota_not_started_without_wifi() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi;
    OTAServiceStub ota;
    StartupLogic logic(&wifi, &ota);

    logic.update();
    TEST_ASSERT_EQUAL(StartupLogic::State::SEARCHING_WIFI, logic.getState());
    TEST_ASSERT_FALSE(ota.begun());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_startup_transition);
    RUN_TEST(test_ota_not_started_without_wifi);
    return UNITY_END();
}
