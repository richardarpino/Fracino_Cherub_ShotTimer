#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "../_common/stubs/OTAServiceStub.h"
#include "../_common/stubs/BlockerStub.h"
#include "../_common/stubs/MachineProviderStub.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Services/WiFiService.h"
#include "../../lib/Services/WiFiService.cpp"
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/StartupLogic.h"
#include "../../lib/Logic/StartupLogic.cpp"
#include "../../lib/Sensors/Registry/RegistrySwitch.h"
#include <stdio.h>

void test_startup_factory_coordination() {
    WiFi.setStatus(WL_IDLE_STATUS);
    SensorDispatcher registry;
    WiFiService wifi(&registry, "dummy", "dummy");
    OTAServiceStub ota;
    BlockerStub warmingUp;
    MachineProviderStub factory(&wifi, &ota, &warmingUp);
    StartupLogic logic(&factory, &registry);

    TEST_ASSERT_FALSE(logic.isActive());
    TEST_ASSERT_FALSE(factory.otaCreated());
    
    // Initial uptime
    registry.publish<SystemUptimeReading>(0.0f);

    // 1. Initial State: SEARCHING_WIFI
    registry.update(); // Initial poll
    logic.update(); 
    TEST_ASSERT_EQUAL_STRING("WiFi", logic.getStatus().title);
    
    // 2. Connect WiFi
    WiFi.setStatus(WL_CONNECTED);
    registry.publish<SystemUptimeReading>(0.1f); // 100ms
    wifi.update();     // Publishes WiFiStatus=1.0 to registry
    registry.update(); // Updates cache
    logic.update();    // Sees WiFiStatus=1.0 via RegistrySwitch
    TEST_ASSERT_FALSE(factory.otaCreated());

    // 3. Hold wait (3s) -> Transitions to OTA_STARTING
    registry.publish<SystemUptimeReading>(3.1f); 
    wifi.update(); 
    registry.update();
    logic.update();
    TEST_ASSERT_TRUE(factory.otaCreated());
    TEST_ASSERT_EQUAL_STRING("OTA STUB", logic.getStatus().title);
    TEST_ASSERT_FALSE(logic.isActive()); 

    // 4. OTA Ready (Listening) -> Transitions to WARMING_UP state
    registry.publish<OTAStatus>(StatusMessage("OTA", "ON", 100.0f, false));
    warmingUp.setTitle("Warming Up...");
    registry.update();
    logic.update();
    TEST_ASSERT_EQUAL_STRING("Warming Up...", logic.getStatus().title);
    TEST_ASSERT_FALSE(logic.isActive()); 
    
    // 5. Warming Up Finished -> READY state
    registry.publish<WarmingUpStatus>(StatusMessage("Ready", "READY", 100.0f, false));
    registry.update();
    logic.update();
    TEST_ASSERT_EQUAL_STRING("Ready", logic.getStatus().title);
    
    TEST_ASSERT_TRUE(logic.isActive());
    TEST_ASSERT_TRUE(logic.justStarted());
}

void test_startup_transition_loop_timing() {
    SensorDispatcher registry;
    WiFi.setStatus(WL_CONNECTED);
    WiFiService wifi(&registry, "dummy", "dummy");
    OTAServiceStub ota; ota.setActive(true);
    BlockerStub warmingUp; warmingUp.setActive(false);
    MachineProviderStub factory(&wifi, &ota, &warmingUp);
    StartupLogic logic(&factory, &registry);

    // Initial state: SEARCHING_WIFI
    registry.publish<SystemUptimeReading>(0.0f);
    registry.update();
    logic.update(); 
    TEST_ASSERT_FALSE(logic.justStarted());

    // 1. Progress to WARMING_UP
    // Frame 1: SEARCHING -> WIFI_STABLE
    registry.publish<SystemUptimeReading>(0.1f);
    wifi.update(); 
    registry.update();
    logic.update(); 
    
    // Frame 2: Wait in WIFI_STABLE
    registry.publish<SystemUptimeReading>(2.0f);
    wifi.update();
    registry.update();
    logic.update();
    
    // Frame 3: WIFI_STABLE -> OTA_STARTING (after 3000ms from Frame 1)
    registry.publish<SystemUptimeReading>(3.2f);
    wifi.update();
    registry.update();
    logic.update();
    
    // Frame 4: OTA_STARTING -> WARMING_UP
    registry.publish<OTAStatus>(StatusMessage("OTA", "ON", 100.0f, false));
    registry.update();
    logic.update();
    
    TEST_ASSERT_FALSE(logic.justStarted());
    TEST_ASSERT_FALSE(logic.isActive());

    // 2. Simulate the frame where Warming Up FINISHES
    registry.publish<WarmingUpStatus>(StatusMessage("Ready", "READY", 100.0f, false));
    
    // Frame 5: WARMING_UP -> READY (Sets isActive=true, justStarted=true)
    registry.update();
    logic.update(); 
    
    // Frame 6: 
    TEST_ASSERT_TRUE(logic.justStarted()); 
    
    // logic.update() -> sets _lastActive = true, sets _justStarted = false
    logic.update();
    TEST_ASSERT_FALSE(logic.justStarted());
}

void test_double_update_edge_loss_warning() {
    SensorDispatcher registry;
    WiFi.setStatus(WL_CONNECTED);
    WiFiService wifi(&registry, "dummy", "dummy");
    OTAServiceStub ota; ota.setActive(true);
    BlockerStub warmingUp; warmingUp.setActive(true);
    MachineProviderStub factory(&wifi, &ota, &warmingUp);
    StartupLogic logic(&factory, &registry);

    // 1. Advance to READY frame
    registry.publish<SystemUptimeReading>(0.0f); registry.update(); logic.update();
    registry.publish<SystemUptimeReading>(0.1f); wifi.update(); registry.update(); logic.update(); // WIFI_STABLE
    registry.publish<SystemUptimeReading>(3.2f); wifi.update(); registry.update(); logic.update(); // OTA_STARTING
    registry.publish<OTAStatus>(StatusMessage("OTA", "ON", 100.0f, false));
    registry.update(); logic.update();                                // WARMING_UP
    
    // 2. The frame where it transitions to READY
    registry.publish<WarmingUpStatus>(StatusMessage("Ready", "READY", 100.0f, false));
    registry.update();
    logic.update(); 
    TEST_ASSERT_TRUE(logic.justStarted()); 

    logic.update();
    
    // 4. Verification: Edge is correctly "lost" (processed) by the second update
    TEST_ASSERT_FALSE(logic.justStarted()); 
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_startup_factory_coordination);
    RUN_TEST(test_startup_transition_loop_timing);
    RUN_TEST(test_double_update_edge_loss_warning);
    return UNITY_END();
}
