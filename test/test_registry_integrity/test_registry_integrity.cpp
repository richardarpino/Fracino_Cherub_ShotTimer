#include <unity.h>
#include "../../lib/Logic/SensorDispatcher.h"
#include "../../lib/Interfaces/SensorTags.h"
#include "../../lib/Interfaces/ITagProcessor.h"
#include <set>
#include <string>

// Mock Processor for testing the verification logic itself
class MockProc : public ITagProcessor {
public:
    void update() override {}
};

/**
 * Recursive Integrity Checker
 * Uses template metaprogramming to crawl the Children TagLists at test-time.
 */
template<typename T>
void verifyTag(ISensorRegistry* registry, std::set<std::string>& verified) {
    std::string name = T::NAME;
    
    // Avoid infinite recursion or redundant checks
    if (verified.count(name)) return;
    verified.insert(name);

    printf("DEBUG: Verifying Tag [%s]\n", name.c_str());

    // Recursively verify ALL children
    verifyTagList(registry, verified, typename T::Children{});
}

// Specialization for empty list
void verifyTagList(ISensorRegistry* registry, std::set<std::string>& verified, TagList<>) {}

// Recursive worker for TagList
template<typename T, typename... Rest>
void verifyTagList(ISensorRegistry* registry, std::set<std::string>& verified, TagList<T, Rest...>) {
    // 1. Every child MUST have a processor registered (unless it's a hardware source, 
    // but in our architecture, Children are ALMOST ALWAYS derived processors)
    
    // Exception: If the tag is a root sensor (like BoilerPressure), it doesn't need a processor.
    // However, if it's in a Children list, it means it's DEPENDENT on something, 
    // so it MUST have a processor to perform that dependency logic.
    
    bool hasProc = registry->hasProcessor(T::NAME);
    TEST_ASSERT_TRUE_MESSAGE(hasProc, (std::string("Orphaned Tag detected: ") + T::NAME + " is a dependency but has no Processor").c_str());

    // 2. Recurse into this child's own dependencies
    verifyTag<T>(registry, verified);

    // 3. Continue with rest of the list
    verifyTagList(registry, verified, TagList<Rest...>{});
}

void test_registry_integrity_wave2() {
    SensorDispatcher registry;
    
    // Mock Processors to simulate a "wired" system (Wave 2 state)
    MockProc heatingProc;
    MockProc tempProc;
    MockProc shotProc;
    MockProc weightProc;
    MockProc warmingProc;
    MockProc safetyProc;

    // These represent the attachProcessor calls in MachineFactory
    registry.attachProcessor<HeatingCycleReading>(&heatingProc);
    registry.attachProcessor<BoilerTempReading>(&tempProc);
    registry.attachProcessor<ShotTimeReading>(&shotProc);
    registry.attachProcessor<TaredWeightReading>(&weightProc);
    registry.attachProcessor<WarmingUpStatus>(&warmingProc);
    registry.attachProcessor<BoilerSafetyStatus>(&safetyProc);

    std::set<std::string> verified;

    // We start verification from "Root" (Hardware-published) tags.
    // If a tag is a child of these, it must have a processor.
    verifyTag<BoilerPressureReading>(&registry, verified);
    verifyTag<PumpReading>(&registry, verified);
    
    // Verify that we actually checked the expected deep graph
    TEST_ASSERT_TRUE(verified.count("BoilerPressure"));
    TEST_ASSERT_TRUE(verified.count("BoilerTemp"));
    TEST_ASSERT_TRUE(verified.count("HeatingCycles"));
    TEST_ASSERT_TRUE(verified.count("WarmingUp"));
    TEST_ASSERT_TRUE(verified.count("ShotTime"));
}

void test_registry_integrity_fails_on_missing() {
    SensorDispatcher registry;
    // Intentionally missing setup for BoilerTempReading
    
    std::set<std::string> verified;
    
    // This should fail because BoilerTempReading is a child of BoilerPressureReading
    // and we haven't attached a processor for it.
    // NOTE: In a real Unity test, we can't easily catch a TEST_ASSERT_TRUE failure 
    // inside a test to prove it works, but we can verify it during development.
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_registry_integrity_wave2);
    return UNITY_END();
}
