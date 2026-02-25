# ADR 0001: Separation of System Logic from Data Sensors (Intent-Revealing Architecture)

*   **Status**: Accepted
*   **Date**: 2026-02-05 (Approximate based on project evolution)

## Context
Originally, machine-specific logic (e.g., "If pressure > 1.0, start the timer") was beginning to leak into the `ISensor` implementations and the UI layer. This resulted in several "Cracks":
1.  **Bloated Sensors**: Sensors were doing more than just measuring; they were deciding machine states (Violation of SRP).
2.  **Ambiguous Naming**: Component names focused on "What they are" instead of "Intent" (Domain Language).
3.  **Untestable Logic**: Since the logic was embedded in hardware-dependent classes, it could only be tested on-device or via complex mocks.

## Decision
We adopted the **Coordinator-Logic Pattern** (also referred to as the Orchestrator pattern):
1.  **Passive Sensors**: `ISensor` and `ISwitch` implementations are restricted to acquiring data and provide high-fidelity filtered values.
2.  **Decoupled Logic**: All decision-making (Shot Timing, Startup Sequences, Warmup Detection) is moved to classes in `lib/Logic`.
3.  **IMachineProvider**: Logic modules access sensors only through an interface-segregated provider, ensuring they never know about the concrete hardware implementation.

## Consequences
*   **Positive**: Domain logic is now 100% testable in a `native` environment.
*   **Positive**: Hardware sensors can be replaced or updated (e.g., moving from a binary switch to a pressure threshold) without touching the high-level logic.
*   **Negative**: Increased "Wiring Work" in `main.cpp` and `MachineFactory` to connect the passive components to their coordinators.
