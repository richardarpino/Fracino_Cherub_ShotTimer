#include <unity.h>
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "Interfaces/SensorTags.h"
#include "Sensors/Registry/RegistrySwitch.h" // Does not exist yet

// Simple mock for SensorTags if needed, but we can use real ones
struct MockPumpTag {
    using DataType = Reading;
    using Children = TagList<>;
    static constexpr const char* NAME = "MockPump";
    static SensorMetadata getMetadata() {
        return SensorMetadata(
            Reading(0.0f, "", "OFF", 0, false),
            Reading(1.0f, "", "ON", 0, false),
            Reading(0.0f, "", "OFF", 0, false),
            Reading(0.0f, "", "ERR", 0, true)
        );
    }
};

void test_registry_switch_independent_edges() {
    SensorDispatcher registry;
    
    // Two switches consuming from the same tag
    RegistrySwitch<MockPumpTag> switchA(&registry);
    RegistrySwitch<MockPumpTag> switchB(&registry);

    // 1. Initial State
    registry.publish<MockPumpTag>(Reading(0.0f, "", "OFF", 0, false));
    switchA.update();
    switchB.update();
    TEST_ASSERT_FALSE(switchA.isActive());
    TEST_ASSERT_FALSE(switchB.isActive());

    // 2. Transition to ON
    registry.publish<MockPumpTag>(Reading(1.0f, "", "ON", 0, false));
    
    // Update A
    switchA.update();
    TEST_ASSERT_TRUE(switchA.isActive());
    TEST_ASSERT_TRUE(switchA.justStarted());

    // Update B later
    switchB.update();
    TEST_ASSERT_TRUE(switchB.isActive());
    // This is the core requirement: B should ALSO see justStarted=true 
    // even though A already "processed" the frame.
    TEST_ASSERT_TRUE(switchB.justStarted());

    // 3. Transition to OFF
    registry.publish<MockPumpTag>(Reading(0.0f, "", "OFF", 0, false));
    switchB.update();
    TEST_ASSERT_FALSE(switchB.isActive());
    TEST_ASSERT_TRUE(switchB.justStopped());

    switchA.update();
    TEST_ASSERT_FALSE(switchA.isActive());
    TEST_ASSERT_TRUE(switchA.justStopped());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_registry_switch_independent_edges);
    return UNITY_END();
}
