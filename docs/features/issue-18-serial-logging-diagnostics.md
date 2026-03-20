# Feature Planning: Serial Logging & Boot Diagnostics (Issue #18)

## Problem Statement
As the codebase grew in complexity, "silent" startup failures (e.g., hanging on a blank screen or failing to connect to WiFi) became difficult to diagnose. Because the `MachineFactory` was initialized in the global static scope, it ran before the `Serial` port was initialized in `setup()`, making early failures invisible.

## Goal
Formalize the boot process to ensure early failures are visible via Serial and that the startup sequence follows clear, logged milestones. Establish a `VERBOSE_BOOT` mechanism for detailed sub-system tracking.

---

## Implementation Strategy

### 1. Shift to Pointer-based Factory
The `MachineFactory` is moved from the global static scope into the explicit `setup()` function.
- **Order of Operations**: `Serial.begin()` is called first, followed by factory construction.
- **Persistence**: Access is maintained in `loop()` via a global `MachineFactory*` pointer.

### 2. Milestone Logging (`VERBOSE_BOOT`)
A set of 10 distinct milestones are established to track the system lifecycle:

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

### 3. Sub-system Instrumentation
Detailed logs are added to critical services:
- **WiFiService**: Logs SSID, connection attempts, and signal strength.
- **OTAService**: Logs port allocation and initialization status.
- **LVGLPainter**: Logs theme application and screen registration.

---

## Technical Details
- **Compile-time Toggle**: Controlled via `-D VERBOSE_BOOT=1` in `platformio.ini`.
- **Memory Impact**: Minimal (uses heap for the factory instead of static storage; minor string overhead).
- **Diagnostics**: If the device hangs, the Serial log provides the exact milestone where the failure occurred.

## Design Decisions & ADRs
- **ADR-0009**: Formal Boot Sequence Diagnostics.
