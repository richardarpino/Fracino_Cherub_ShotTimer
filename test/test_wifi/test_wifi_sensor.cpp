#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "../../lib/Services/WiFiService.h"
#include "../../lib/Services/WiFiService.cpp"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../../lib/Interfaces/IBlocker.h"

void test_wifi_blocker_reporting() {
    SensorDispatcher registry;
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiService service(&registry, "dummy", "dummy");
    IBlocker* blocker = (IBlocker*)&service;
    
    // Process should be indeterminate/connecting
    TEST_ASSERT_EQUAL_STRING("CONNECTING...", blocker->getStatus().message);
    TEST_ASSERT_EQUAL_FLOAT(-1.0f, blocker->getStatus().progress);
    TEST_ASSERT_FALSE(blocker->getStatus().isFailed);

    // Connected state
    WiFi.setStatus(WL_CONNECTED);
    WiFi.setIP("192.168.1.50");
    blocker->update();
    TEST_ASSERT_EQUAL_STRING("CONNECTED: 192.168.1.50", blocker->getStatus().message);
    TEST_ASSERT_TRUE(blocker->isActive());
    TEST_ASSERT_EQUAL_FLOAT(100.0f, blocker->getStatus().progress);
    
    // Verify registry publishing
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, registry.getLatest<WiFiTag>().progress);
}

void test_wifi_blocker_failure() {
    SensorDispatcher registry;
    WiFiService service(&registry, "dummy", "dummy");
    IBlocker* blocker = (IBlocker*)&service;
    WiFi.setStatus(WL_CONNECT_FAILED);
    
    blocker->update();
    TEST_ASSERT_EQUAL_STRING("CONNECTION FAILED", blocker->getStatus().message);
    TEST_ASSERT_TRUE(blocker->getStatus().isFailed);
    TEST_ASSERT_FALSE(blocker->isActive());
    
    // Verify registry publishing (Indeterminate/Disconnected is -1.0f)
    TEST_ASSERT_FLOAT_WITHIN(0.1f, -1.0f, registry.getLatest<WiFiTag>().progress);
}

void test_wifi_construction_starts_wifi() {
    SensorDispatcher registry;
    WiFiService service(&registry, "TestSSID", "TestPass");

    TEST_ASSERT_TRUE(WiFi.wasBeginCalled());
    TEST_ASSERT_EQUAL_STRING("TestSSID", WiFi.getSsid());
    TEST_ASSERT_EQUAL_STRING("TestPass", WiFi.getPass());
}

void test_wifi_switch_behavior() {
    SensorDispatcher registry;
    WiFiService service(&registry, "dummy", "dummy");
    ISwitch* sw = (ISwitch*)&service;

    WiFi.setStatus(WL_DISCONNECTED);
    sw->update();
    TEST_ASSERT_FALSE(sw->isActive());

    WiFi.setStatus(WL_CONNECTED);
    sw->update();
    TEST_ASSERT_TRUE(sw->isActive());
    TEST_ASSERT_TRUE(sw->justStarted());

    sw->update();
    TEST_ASSERT_FALSE(sw->justStarted()); // Latch should clear
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_blocker_reporting);
    RUN_TEST(test_wifi_blocker_failure);
    RUN_TEST(test_wifi_construction_starts_wifi);
    RUN_TEST(test_wifi_switch_behavior);
    return UNITY_END();
}
