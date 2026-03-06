#include <unity.h>
#include "Logic/SensorDispatcher.h"
#include "Logic/SensorDispatcher.cpp"
#include "Interfaces/ITagProcessor.h"
#include "Interfaces/SensorTags.h"

// Mock Processor for Testing
class MockDerivedProcessor : public ITagProcessor {
public:
    int callCount = 0;
    SensorDispatcher* registry;

    MockDerivedProcessor(SensorDispatcher* reg) : registry(reg) {}

    void update() override {
        callCount++;
        // Simulate a derived calculation: Just publish the parent value + 1
        Reading parent = registry->getLatest<BoilerPressureTag>();
        registry->publish<HeatingCycleTag>(Reading(parent.value + 1.0f, "", "DERIVED", 0, false, PhysicalQuantity::COUNTER));
    }
};

void test_auto_propagation_missing() {
    SensorDispatcher dispatcher;
    MockDerivedProcessor processor(&dispatcher);

    dispatcher.attachProcessor<HeatingCycleTag>(&processor);

    // Publish parent
    dispatcher.publish<BoilerPressureTag>(Reading(1.0f, "BAR", "", 0, false, PhysicalQuantity::PRESSURE));

    // VERIFY: In the reactive system, the processor should be called immediately
    TEST_ASSERT_EQUAL(1, processor.callCount);
    
    // Child tag should be updated (Parent value 1.0 + 1.0 = 2.0)
    Reading child = dispatcher.getLatest<HeatingCycleTag>();
    TEST_ASSERT_EQUAL_FLOAT(2.0f, child.value);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_auto_propagation_missing);
    return UNITY_END();
}
