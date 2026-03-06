# ADR 0006: Reactive Tag Processors & Logic Sub-Structuring

## Status
Proposed (March 2026)

## Context
As the codebase evolves towards a data-first architecture, we have introduced "Derived Tags" (e.g., `HeatingCycleTag` derived from `BoilerPressureTag`). Initially, this logic was embedded in active orchestrators like `WarmingUpBlocker`, which led to:
1.  **Temporal Coupling (Clock Races)**: Components might read stale data if they execute before the orchestrator updates the derived tag.
2.  **SRP Violations**: Orchestrators were responsible for both process management and low-level data calculation (e.g., zigzag pressure history).
3.  **Test Complexity**: Calculating derived state inside UI-connected blockers made it harder to unit test the calculation logic in isolation.

## Decision
1.  **Reactive Processors**: Implement `ITagProcessor` components that are managed by the `SensorDispatcher`. These are triggered immediately during the `publish()` call of a parent tag, ensuring all derived state is consistent within the same call stack.
2.  **Explicit Dependency Graph**: Use `TagList<...>` in `SensorTags.h` to explicitly declare which child tags are dependent on a parent tag.
3.  **Logic Sub-Structuring**: To differentiate between different types of logic, the `lib/Logic` directory is structured into functional sub-folders:
    -   `lib/Logic/Processors/`: Passive components that transform telemetry into derived telemetry/readings (e.g., `HeatingCycleProcessor`).
    -   `lib/Logic/Triggers/`: Components that transform continuous telemetry into discrete events/switches (e.g., `ThresholdSwitch`, `LogicalAnd`).
    -   `lib/Logic/`: (Root) Active orchestrators and domain-specific state managers.

## Consequences
-   **Pros**:
    -   Eliminates "frame delay" for derived calculations.
    -   Cleaner separation of concerns (Calculators vs. Triggers vs. Orchestrators).
    -   Improved testability: Processors can be unit tested by simply pumping parent data and checking registry output.
    -   Enables automated documentation of the data flow/dependency graph.
-   **Cons**:
    -   Slightly more complex `SensorDispatcher` (handles template recursion for resolution).
    -   Deepening folder structure requires more careful relative include management.

## Alternatives Considered
-   **Active Orchestration**: Keeping calculations inside `ShotMonitor` or `WarmingUpBlocker`. Rejected due to temporal coupling.
-   **Event-Based System**: Using a traditional Pub/Sub event bus. Rejected to maintain the "Registry as Single Source of Truth" and to keep the memory footprint low for the ESP32.
-   **Manual Update Loop**: Calling `processor.update()` manually in `main.cpp`. Rejected as it is error-prone and doesn't solve the ordering problem (temporal coupling) without strict manual sequencing.
