# ADR 0010: Passive Blocker Pattern (Decoupled Logic Gates)

*   **Status**: Accepted
*   **Date**: 2026-03-19

## Context
Refactoring `OTAService` to improve SRP exposed a critical architectural dependency: background services (like `ArduinoOTA.handle()`) must be explicitly updated in the main loop even when decoupled from the UI gates. An initial over-reaching refactor broke the OTA update loop because `MachineFactory` was not correctly managing the lifecycle of the decoupled service.

## Decision
We formally adopt the **Passive Blocker Pattern** for transient background services:
1.  **Service (SRP Worker)**: A pure background worker that performs active hardware tasks and publishes its high-level status to the `ISensorRegistry`.
2.  **Blocker (Logic Gate)**: A pure passive observer that pulls from the Registry to implement the `IBlocker` logic gate. 
3.  **Centralized Lifecycle**: `MachineFactory` is responsible for updating both the active Service and the passive Blocker via a centralized `update()` method called from the main loop.

## Consequences
*   **Positive**: Perfect Single Responsibility (SRP); the service doesn't know about UI blocking, and the blocker doesn't know about hardware protocols.
*   **Positive**: Improved Testability; logic gates can be 100% verified in `native` environments using registry stubs.
*   **Positive**: Safe Visual Previews; the simulator can snapshot blocker screens without instantiating complex hardware services.
*   **Positive**: Predictable Execution; centralizing updates in `MachineFactory` ensures guaranteed processing for all background tasks.
