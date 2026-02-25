# ADR 0003: Segregation of Telemetry (ISensor) and Startup Gates (IBlocker)

*   **Status**: Accepted
*   **Date**: 2026-02-23

## Context
Early in the project, we attempted to treat everything that provided "Status" as a sensor. This included WiFi connectivity, OTA updates, and Boiler Warmup.
1.  **Interface Mismatch**: Startup services (WiFi/OTA) were forced to implement `ISensor`, returning dummy `Reading` objects just to satisfy the interface.
2.  **Logic Bloat**: `StartupLogic` had to parse "magic numbers" or string values from sensor readings to determine if a connection was established.
3.  **UI Fragmentation**: We lacked a standardized way to show "Progress" and "Status Messages" for these transient startup phases.

## Decision
We formalized a split between **Telemetry** and **Blocking Gates**:
1.  **`ISensor` (Telemetry)**: Reserved for continuous, high-fidelity data streams (Pressure, Temperature, Weight).
2.  **`IBlocker` (Startup Gates)**: A specific interface for transient services that must complete before the machine enters "READY" state.
    *   Returns a `BlockerStatus` object containing `progress` (float), `message` (String), and `isFinished` (bool).
3.  **Specialized Widgets**: Created `BlockerWidget` to consume `IBlocker` and `SensorWidget`/`GaugeWidget` to consume `ISensor`.

## Consequences
*   **Positive**: `StartupLogic` became a clean orchestrator of `IBlocker` states without needing to know implementation details.
*   **Positive**: Zero "Dummy Readings" in the telemetry stream.
*   **Positive**: Consistent UI experience during boot (Dual-label status bars).
*   **Negative**: Services that act as both (like a sensor that blocks until warm) require careful implementation of both interfaces or a proxy logic.
