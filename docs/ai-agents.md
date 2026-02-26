# Agentic Rules of Engagement

This document is specifically for AI Agentic systems (like Antigravity) contributing to this repository. These rules ensure that AI efficiency doesn't compromise the project's architectural integrity.

## 1. Contextual Awareness
*   **Read Before Write**: Before proposing any structural change, you **must** read `docs/principles.md`.
*   **Check ADRs**: (Future) Look for existing Architecture Decision Records in `docs/adr/` to understand why a component is structured the way it is.

## 2. TDD Enforcement
*   **No Test, No PR**: You must provide a pass/fail log from at least the `native` test suite for any logic modification.
*   **Regression Testing**: If you modify a shared library (e.g., `StringUtils` or `FilteredSensor`), you must run the *entire* native test suite.

### 3. Abstraction & SOLID Preservation
*   **Respect the Boundary**: Never bypass an interface (`ISwitch`, `IBlocker`, `IRawSource`) to access hardware directly from logic.
*   **Sensor = Hardware**: All physical observers must inherit from `HardwareSensor`. Never inherit this for logical calculations.
*   **Registry Producers**: New logic components must be "Producers." They should calculate data and utilize `ISensorRegistry::publish<Tag>()` to make it available to the UI.
*   **Maintain SRP**: If you find yourself adding history-tracking to a logic component or state-logic to a sensor, you are breaking SRP. Create a new Orchestrator in `lib/Logic/` instead.
*   **MachineFactory (DI)**: All object instantiation should happen in the `MachineFactory`. Always favor constructor injection over direct instantiation of dependencies.

## 4. Domain-Driven Design (DDD)
*   **Ubiquitous Language**: Follow the domain naming established in the codebase. If you are adding a feature for "taring a scale," ensure the names reflect the user domain (e.g., `TaredWeightSensor`) rather than implementation details.
*   **Walkthroughs**: After completing a task, update the relevant `docs/` file if you have evolved a new principle or pattern.
*   **The Gallery**: If you change a UI element, you must run the `simulator` test to regenerate the documentation gallery and verify no visual regressions.

## 5. Decision Logging
If you make a significant architectural decision (e.g., introducing a new layer or changing a core interface), you should create a new ADR entry in `docs/adr/`.
