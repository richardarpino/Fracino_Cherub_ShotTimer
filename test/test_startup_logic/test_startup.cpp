#include <unity.h>
#include "../_common/stubs/WiFi.h"
#include "../_common/stubs/WiFi.cpp"
#include "../_common/stubs/OTAServiceStub.h"
#include "../_common/stubs/MachineProviderStub.h"
#include "../_common/stubs/Arduino.h"
#include "../_common/stubs/Arduino.cpp"
#include "Hardware/WiFiSensor.h"
#include "Hardware/WiFiSensor.cpp"
#include "Logic/StartupLogic.h"
#include "Logic/StartupLogic.cpp"

void test_startup_factory_coordination() {
    WiFi.setStatus(WL_IDLE_STATUS);
    WiFiSensor wifi("dummy", "dummy");
    OTAServiceStub ota;
    MachineProviderStub factory(&wifi, &wifi, &ota);
    StartupLogic logic(&factory);

    TEST_ASSERT_FALSE(logic.isActive());
    TEST_ASSERT_FALSE(factory.otaCreated());
    
    // Connect WiFi
    WiFi.setStatus(WL_CONNECTED);
    setMillis(100); wifi.update(); logic.update();
    TEST_ASSERT_FALSE(factory.otaCreated());

    // Hold wait (3s)
    setMillis(3100); wifi.update(); logic.update();
    TEST_ASSERT_TRUE(factory.otaCreated()); // Factory should have created OTA now
    TEST_ASSERT_FALSE(logic.isActive()); 

    // OTA Ready (Listening) -> READY state
    ota.setActive(true);
    logic.update();
    
    TEST_ASSERT_TRUE(logic.isActive());
    TEST_ASSERT_TRUE(logic.justStarted());
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_startup_factory_coordination);
    return UNITY_END();
}
