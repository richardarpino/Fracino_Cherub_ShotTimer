#include <unity.h>
#include <Arduino.h>
#include "../_common/stubs/Arduino.cpp"
#include "../../lib/Services/WarmingUpBlocker.h"
#include "../../lib/Services/WarmingUpBlocker.cpp"
#include <SensorStub.h>
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Logic/SensorDispatcher.cpp"
#include "../../lib/Logic/Processors/WarmingUpProcessor.h"
#include "../../lib/Logic/Processors/WarmingUpProcessor.cpp"

/**
 * PROVE: Missing tag in registry blocks the machine forever.
 */
void test_lockup_missing_tag() {
    SensorDispatcher registry;
    WarmingUpBlocker blocker(&registry);
    
    // Tag never published (-1.0f progress)
    blocker.update();
    
    // FAIL-SAFE: Should NOT block forever if tag is missing
    TEST_ASSERT_TRUE_MESSAGE(blocker.isActive(), "Machine is UNBLOCKED when tag is missing (Fail-Safe)");
}

/**
 * PROVE: A 'Failed' status message unblocks the machine.
 */
void test_lockup_failed_status() {
    SensorDispatcher registry;
    WarmingUpBlocker blocker(&registry);
    
    // Simulate a failure
    registry.publish<WarmingUpStatus>(StatusMessage("Warmup Error", "Sensor Disconnected", 50.0f, true));
    blocker.update();
    
    TEST_ASSERT_TRUE_MESSAGE(blocker.isActive(), "Machine is UNBLOCKED when status is FAILED (Fail-Safe)");
}

/**
 * PROVE: If progress is 99.9%, it blocks forever.
 */
void test_lockup_near_miss() {
    SensorDispatcher registry;
    WarmingUpBlocker blocker(&registry);
    
    registry.publish<WarmingUpStatus>(StatusMessage("Warming Up...", "Almost there", 99.9f, false));
    blocker.update();
    
    TEST_ASSERT_FALSE_MESSAGE(blocker.isActive(), "Machine is blocked at 99.9% progress");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_lockup_missing_tag);
    RUN_TEST(test_lockup_failed_status);
    RUN_TEST(test_lockup_near_miss);
    return UNITY_END();
}
