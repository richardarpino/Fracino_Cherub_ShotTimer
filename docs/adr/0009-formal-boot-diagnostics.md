# ADR 0009: Formal Boot Sequence Diagnostics

## Status
Accepted

## Context
As the codebase grew in complexity, "silent" startup failures (e.g., hanging on a blank screen or failing to connect to WiFi) became difficult to diagnose. Because the `MachineFactory` was initialized in the global static scope, it ran before the `Serial` port was initialized in `setup()`, making early failures invisible.

## Decision
We have moved the system initialization from the global scope into the explicit `setup()` function. This ensures that `Serial.begin()` is called first, allowing all subsequent initialization steps to be logged.

We introduced a `VERBOSE_BOOT` system consisting of:
1.  **Managed Milestones**: 10 distinct milestones (`001` - `010`) covering the lifecycle of the factory, display, and painter.
2.  **Pointer-based Factory**: The `MachineFactory` is now a global pointer initialized in `setup()`.
3.  **Compile-time Toggle**: A `-D VERBOSE_BOOT=1` flag in `platformio.ini`.

## Milestone Mapping

| Code | Component | Description |
| :--- | :--- | :--- |
| `001` | `MachineFactory` | Registering Widget classes |
| `002` | `MachineFactory` | Seeding Type-Safe Whitelists |
| `003` | `MachineFactory` | Wiring Hardware Sensor Sources |
| `004` | `MachineFactory` | Attaching Reactive Processors |
| `005` | `MachineFactory` | Initializing UI Widget Factory |
| `006` | `ShotDisplay` | Initializing TFT Hardware |
| `007` | `ShotDisplay` | Initializing LVGL Core |
| `008` | `ShotDisplay` | Allocating Display Buffers |
| `009` | `ShotDisplay` | Registering Display Driver |
| `010` | `LVGLPainter` | Initializing UI Layout & Theme |

## Consequences
- **Pros**: Clear visibility into "Hanging" states; easier remote debugging via Serial; centralized startup sequence.
- **Cons**: Requires heap allocation for the factory; minor increase in binary size due to strings (mitigated by `#ifdef` toggles).
