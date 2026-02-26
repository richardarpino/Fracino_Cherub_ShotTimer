# ADR 0004: Registry Producer Pattern

## Status
Accepted (2026-02-26)

## Context
Previously, logical data components (like `ShotTimer` or `BoilerTemperature`) were treated as "Virtual Sensors" inheriting from a common `ISensor` interface. This led to:
1.  **Polling Responsibility Ambiguity**: The UI had to poll these virtual sensors, which in turn polled other hardware sensors, creating deep execution chains.
2.  **Interface Pollution**: Logical components were forced to implement "getReading()" even when they had specialized APIs (`start()`, `tare()`) that the UI shouldn't touch.
3.  **Synchronization Issues**: If a widget updated before the parent logic, it might see stale data within the same frame.

## Decision
We moved to a **Registry Producer Pattern**:
1.  **HardwareSensor** is a strict base class for physical hardware observation only.
2.  **Logic Components** are stripped of sensor interfaces. They are manipulated by **Orchestrators** (e.g., `ScaleLogic`).
3.  **The Registry** (`ISensorRegistry`) becomes the single source of truth for both physical and logical data.
4.  **Producers**: Orchestrators are responsible for calculating high-level results and **pushing** them into the Registry cache via `publish<Tag>()`.
5.  **Tags as Domain Contracts**: Type-Tags are no longer dumb markers; they carry static `SensorMetadata`. Documentation and UI Widgets use the Tag to determine scaling, units, and ranges.

## Consequences
- **Positive**: Strict bifurcation between "Hardware" (observers) and "Logic" (calculators).
- **Positive**: UI widgets are completely decoupled; they only know "Type Tags" and the Registry.
- **Positive**: Meta-restoration; by moving metadata to Tags, logical components (like `ShotTimer`) regain their descriptive metadata without needing an instance.
- **Positive**: Frame-synchronized data; the Registry cache ensures all components see the same values for the duration of the loop.
- **Negative**: Adds a "Registry Registration" step in the `MachineFactory` or Orchestrator wiring.
- **Negative**: Logic components now require an explicit "Update/Publish" cycle.
