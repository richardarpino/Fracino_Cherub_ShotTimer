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
#include "Logic/StartupLogic.h"
#include "Logic/StartupLogic.cpp"
#include <stdio.h>

void test_startup_factory_coordination() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiService wifi("dummy", "dummy");
    OTAServiceStub ota;
    BlockerStub warmingUp;
    MachineProviderStub factory(&wifi, &ota, &warmingUp);
    StartupLogic logic(&factory);

    TEST_ASSERT_FALSE(logic.isActive());
    TEST_ASSERT_FALSE(factory.otaCreated());
    logic.update(); // Initialize internal pointers
    TEST_ASSERT_EQUAL_STRING("WiFi", logic.getStatus().title.c_str());
    
    // Connect WiFi
    WiFi.setStatus(WL_CONNECTED);
    setMillis(100); wifi.update(); logic.update();
    TEST_ASSERT_FALSE(factory.otaCreated());

    // Hold wait (3s) -> Transitions to OTA_STARTING
    setMillis(3100); wifi.update(); logic.update();
    TEST_ASSERT_TRUE(factory.otaCreated());
    TEST_ASSERT_EQUAL_STRING("OTA STUB", logic.getStatus().title.c_str());
    TEST_ASSERT_FALSE(logic.isActive()); 

    // OTA Ready (Listening) -> Transitions to WARMING_UP state
    ota.setActive(true);
    warmingUp.setTitle("Warming Up...");
    logic.update();
    TEST_ASSERT_EQUAL_STRING("Warming Up...", logic.getStatus().title.c_str());
    TEST_ASSERT_FALSE(logic.isActive()); 
    
    // Warming Up Finished -> READY state
    warmingUp.setActive(true);
    logic.update();
    TEST_ASSERT_EQUAL_STRING("Ready", logic.getStatus().title.c_str());
    
    TEST_ASSERT_TRUE(logic.isActive());
    TEST_ASSERT_TRUE(logic.justStarted());
}

void test_startup_transition_loop_timing() {
    WiFi.setStatus(WL_CONNECTED);
    WiFiService wifi("dummy", "dummy");
    OTAServiceStub ota; ota.setActive(true);
    BlockerStub warmingUp; warmingUp.setActive(false);
    MachineProviderStub factory(&wifi, &ota, &warmingUp);
    StartupLogic logic(&factory);

    printf("\nDEBUG: Started Loop Timing Test\n"); fflush(stdout);

    // Initial state: SEARCHING_WIFI
    setMillis(0);
    logic.update(); 
    TEST_ASSERT_FALSE(logic.justStarted());

    // 1. Progress to WARMING_UP
    // Frame 1: SEARCHING -> WIFI_STABLE
    setMillis(100); wifi.update(); 
    logic.update(); printf("DEBUG: Frame 1 logic ACTIVE=%d\n", logic.isActive()); fflush(stdout);
    
    // Frame 2: Wait in WIFI_STABLE
    setMillis(2000); logic.update();
    
    // Frame 3: WIFI_STABLE -> OTA_STARTING (after 3000ms from Frame 1)
    setMillis(3200); logic.update();
    printf("DEBUG: Frame 3 logic ACTIVE=%d\n", logic.isActive()); fflush(stdout);
    
    // Frame 4: OTA_STARTING -> WARMING_UP (because ota.isActive() is true)
    logic.update();
    printf("DEBUG: Frame 4 logic ACTIVE=%d\n", logic.isActive()); fflush(stdout);
    
    TEST_ASSERT_FALSE(logic.justStarted());
    TEST_ASSERT_FALSE(logic.isActive());

    // 2. Simulate the frame where Warming Up FINISHES
    warmingUp.setActive(true);
    
    // Frame N:
    logic.update(); // Frame 5: WARMING_UP -> READY (Sets isActive=true, justStarted=true)
    printf("DEBUG: Frame 5 logic ACTIVE=%d, JUST_STARTED=%d\n", logic.isActive(), logic.justStarted()); fflush(stdout);
    
    // Frame N+1:
    // if (logic.justStarted()) -> TRUE! 
    TEST_ASSERT_TRUE(logic.justStarted()); 
    
    // Frame N+1 continues:
    // logic.update() -> sets _lastActive = true, sets _justStarted = false
    logic.update();
    printf("DEBUG: Frame 6 logic.update(). ACTIVE=%d, JUST_STARTED=%d\n", logic.isActive(), logic.justStarted()); fflush(stdout);

    // Frame N+2:
    // if (logic.justStarted()) -> FALSE!
    TEST_ASSERT_FALSE(logic.justStarted());
}

void test_double_update_edge_loss_warning() {
    WiFi.setStatus(WL_CONNECTED);
    WiFiService wifi("dummy", "dummy");
    OTAServiceStub ota; ota.setActive(true);
    BlockerStub warmingUp; warmingUp.setActive(true);
    MachineProviderStub factory(&wifi, &ota, &warmingUp);
    StartupLogic logic(&factory);

    // 1. Advance to READY frame
    setMillis(0); logic.update();
    setMillis(100); wifi.update(); logic.update(); // WIFI_STABLE
    setMillis(3200); logic.update();               // OTA_STARTING
    logic.update();                                // WARMING_UP
    
    // 2. The frame where it transitions to READY
    logic.update(); 
    TEST_ASSERT_TRUE(logic.justStarted()); 

    // 3. Second update in same loop (This represents the BUG we just fixed in BlockerWidget)
    logic.update();
    
    // 4. Verification: Edge is lost! 
    // This test documents WHY we removed update() from widgets.
    TEST_ASSERT_FALSE(logic.justStarted()); 
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_startup_factory_coordination);
    RUN_TEST(test_startup_transition_loop_timing);
    RUN_TEST(test_double_update_edge_loss_warning);
    return UNITY_END();
}
