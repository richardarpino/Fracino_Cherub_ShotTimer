#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "../../lib/Services/WiFiService.h"
#include "../../lib/Services/WiFiService.cpp"
#include "../../lib/Logic/Processors/WiFiProcessor.h"
#include "../../lib/Logic/Processors/WiFiProcessor.cpp"
#include "../../lib/Services/WiFiBlocker.h"
#include "../../lib/Services/WiFiBlocker.cpp"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../../lib/Interfaces/IBlocker.h"

void test_wifi_integrated_flow() {
    SensorDispatcher registry;
    WiFi.setStatus(WL_DISCONNECTED);
    
    WiFiService service(&registry, "dummy", "dummy");
    WiFiProcessor processor(&registry);
    WiFiBlocker blocker(&registry);
    
    // Initial state
    service.update(); // Publishes Raw
    processor.update(); // Maps Raw -> Status
    blocker.update(); // Checks Status
    
    TEST_ASSERT_FALSE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("CONNECTING...", blocker.getStatus().message);

    // Connected state
    WiFi.setStatus(WL_CONNECTED);
    WiFi.setIP("192.168.1.50");
    
    service.update();
    processor.update();
    blocker.update();
    
    TEST_ASSERT_TRUE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("CONNECTED: 192.168.1.50", blocker.getStatus().message);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, blocker.getStatus().progress);
}

void test_wifi_failure_flow() {
    SensorDispatcher registry;
    WiFiService service(&registry, "dummy", "dummy");
    WiFiProcessor processor(&registry);
    WiFiBlocker blocker(&registry);

    WiFi.setStatus(WL_CONNECT_FAILED);
    
    service.update();
    processor.update();
    blocker.update();
    
    TEST_ASSERT_TRUE(blocker.isActive()); // Fail-safe
    TEST_ASSERT_TRUE(blocker.getStatus().isFailed);
    TEST_ASSERT_EQUAL_STRING("CONNECTION FAILED", blocker.getStatus().message);
}

void test_wifi_construction_starts_wifi() {
    SensorDispatcher registry;
    WiFiService service(&registry, "TestSSID", "TestPass");

    TEST_ASSERT_TRUE(WiFi.wasBeginCalled());
    TEST_ASSERT_EQUAL_STRING("TestSSID", WiFi.getSsid());
    TEST_ASSERT_EQUAL_STRING("TestPass", WiFi.getPass());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_wifi_integrated_flow);
    RUN_TEST(test_wifi_failure_flow);
    RUN_TEST(test_wifi_construction_starts_wifi);
    return UNITY_END();
}
