#include <unity.h>
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Interfaces/SensorTags.h"
#include "../../lib/Logic/Triggers/ThresholdSwitch.h"
#include "../../lib/Interfaces/ITagProcessor.h"

#include "../../lib/Logic/Processors/BoilerSafetyProcessor.h"

void test_pressure_safety_trigger() {
    SensorDispatcher registry;
    BoilerSafetyProcessor safetyProc(&registry);
    
    // Wire up the processor so it reacts to pressure updates
    registry.attachProcessor<BoilerSafetyStatus>(&safetyProc);

    // Initial state: OK
    registry.publish<BoilerPressureReading>(1.1f);
    TEST_ASSERT_EQUAL_STRING("OK", registry.getLatest<BoilerSafetyStatus>().message);
    TEST_ASSERT_FALSE(registry.getLatest<BoilerSafetyStatus>().isFailed);

    // Trigger state: DANGER (at 2.5 bar)
    registry.publish<BoilerPressureReading>(2.55f);
    TEST_ASSERT_EQUAL_STRING("DANGER: HIGH", registry.getLatest<BoilerSafetyStatus>().message);

    // Hysteresis test: stays active at 2.45 bar (threshold 2.5 - hysteresis 0.1 = 2.4)
    registry.publish<BoilerPressureReading>(2.45f);
    TEST_ASSERT_EQUAL_STRING("DANGER: HIGH", registry.getLatest<BoilerSafetyStatus>().message);

    // Reset test: goes back to OK at 2.3 bar
    registry.publish<BoilerPressureReading>(2.35f);
    TEST_ASSERT_EQUAL_STRING("OK", registry.getLatest<BoilerSafetyStatus>().message);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_safety_trigger);
    return UNITY_END();
}
