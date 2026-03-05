# ADR 0005: Sensor Registry Type-Tags & Metadata

## Status
Accepted (March 2026)

## Context
The previous Sensor Registry used a name-based lookup that forced callers to know whether they were requesting a `Reading` (telemetry) or a `StatusMessage` (process status). This led to split APIs (`getLatest` vs `getLatestStatus`) and accidental misuse. Furthermore, documentation example data was split between tags and hardcoded simulator logic.

Initial attempts to use `if constexpr` for dispatching caused build regressions on specific ESP32 compiler versions due to incomplete dead-code pruning during template instantiation.

## Decision
1.  **Unified Registry API**: Use templated tags (e.g., `BoilerPressureTag`) that carry their required data type as a nested alias (`DataType`).
2.  **Explicit Data Types**:
    -   `Reading`: For continuous telemetry (numeric values, units, scaling).
    -   `StatusMessage`: For discrete system processes (titles, messages, progress bars).
3.  **Tag-Dispatching**: The registry dispatching is handled via classic function overloading (Tag-Dispatching) rather than `if constexpr`. This ensures robust compilation across all C++17 compilers (including embedded variants).
4.  **Tag Inheritance**: Differentiate markers into `BaseTelemetryTag` and `BaseServiceTag` to centralize type definitions and improve semantic clarity.
5.  **Symmetric Tag Metadata**: All tags MUST provide 4 named example states for automated documentation gallery generation.
    -   **Telemetry**: `low`, `high`, `init`, `error`.
    -   **Services**: `pending`, `active`, `ready`, `failed`.

## Consequences
-   **Pros**: Full type safety at the registry gate. Self-documenting tags. Centralized example data allows for "one-click" automated UI documentation. Guaranteed compiler compatibility for embedded targets.
-   **Cons**: Slightly more boilerplate in `SensorTags.h` and registry implementations due to tag-dispatching overloads.

## Alternatives Considered
-   **Traits**: Separating metadata into a different file. Rejected to keep the Tag as a "Single Source of Truth."
-   **Dynamic Vectors for Service States**: Rejected in favor of named fields to achieve symmetric, predictable documentation layouts.
-   **if constexpr**: Initially proposed but rejected due to inconsistent compiler behavior on embedded targets.
