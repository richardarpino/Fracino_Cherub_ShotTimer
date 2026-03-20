#include <unity.h>
#include "../../lib/Services/OTABlocker.h"
#include "../../lib/Services/OTABlocker.cpp"
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "../../lib/Interfaces/IBlocker.h"
#include "../../lib/Interfaces/SensorTags.h"

void test_ota_blocker_reacts_to_status() {
    SensorDispatcher registry;
    OTABlocker blocker(&registry);
    
    // Initial state: Off / Listening (Non-blocking)
    registry.publish<OTAStatus>(StatusMessage("OTA", "LISTENING...", 100.0f, false));
    blocker.update();
    TEST_ASSERT_TRUE(blocker.isActive());

    // Updating state: BLOCKING (Gate CLOSED)
    registry.publish<OTAStatus>(StatusMessage("OTA", "UPDATING: 50%", 50.0f, false));
    blocker.update();
    TEST_ASSERT_FALSE(blocker.isActive());
    TEST_ASSERT_EQUAL_STRING("UPDATING: 50%", blocker.getStatus().message);

    // Error state: NOT BLOCKING (Gate OPEN - fail-safe)
    registry.publish<OTAStatus>(StatusMessage("OTA", "FAILED", 0.0f, true));
    blocker.update();
    TEST_ASSERT_TRUE(blocker.isActive()); 
    TEST_ASSERT_TRUE(blocker.getStatus().isFailed);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_ota_blocker_reacts_to_status);
    return UNITY_END();
}
