# Architectural Pillars

These principles define the structural integrity of the codebase. Adhering to these ensures that hardware changes don't break logic, and logic changes don't crash the UI.

## 1. Domain-Driven Design (DDD) & Ubiquitous Language
**The Rule**: We use a shared, ubiquitous language derived from the espresso machine domain (`WarmingUp`, `ShotTimer`, `PumpSignal`).

*   **Intent-Revealing Names**: Class and variable names must describe *intent*, not just "how" or "what" type they are (e.g., `WarmingUpBlocker` vs. `CycleCounter`).
*   **Bounded Contexts**: The `Logic/` layer should be shielded from the "noise" of hardware. A `ShotTimer` doesn't care about "Pin 25"; it cares about a `PumpSignal` switch being active.

## 2. SOLID: Single Responsibility (SRP)
**The Rule**: Every class must have one, and only one, reason to change.

*   **Sensors**: Measure raw values and filter them. They do NOT decide state transitions.
*   **Decouplers**: `IRawSource` separates the sensor's physical measurement from its logical representation.
*   **Blockers**: Manage a specific startup gate. They do NOT maintain permanent history (Issue #7).

## 3. Dependency Injection (DI) & The Factory
**The Rule**: High-level logic modules MUST NOT instantiate their own dependencies. 

*   **MachineFactory**: This is the single source of truth for Dependency Injection. It wires the concrete hardware to the abstract logic.
*   **Testable DI**: By injecting dependencies (like `ISensor*`) into constructors, we enable the `native` test suite to swap real hardware for stubs (`SensorStub`) instantly.

## 4. Coordinator-Logic Pattern
**The Rule**: Sensors and Switches are passive data pipes. Behavior logic must reside in `Logic/` orchestrators.

*   **Why**: By moving "Machine Logic" (like detecting a shot start or a boiler cycle) into separate classes, we can test it 100% in the `native` environment without ever touching an ESP32 or a display.

## 2. Passive UI (One-Way Data Flow)
**The Rule**: UI Widgets are "Passive Consumers." They should only display data provided to them.

*   **Logic Isolation**: Widgets should never consume "Edge Signals" (like `justStarted()`) if that signal is required for state transitions elsewhere. 
*   **Responsibility**: The `layout` or `MainLoop` should coordinate transitions. The widget just renders what it is told.

## 3. Lazy Provisioning & Provider Abstraction
**The Rule**: Domain logic must never depend directly on a concrete Hardware class (e.g., `WiFiService`). It must depend on an interface (e.g., `IBlocker`) provided via `IMachineProvider`.

*   **IMachineProvider**: This interface is the "Safe Portal" to hardware. Logic modules get what they need from here.
*   **Lazy Loading**: Most services (WiFi, OTA) should only be instantiated when first requested. Instantiation *is* the activation.

## 4. Hardware Independence
**The Rule**: Keep domain logic "Generic." 

*   The core logic shouldn't know it's running on an ESP32. If you find yourself including `Arduino.h` in a `Logic/` file, ask if you can move that hardware-specific bit into a `Sensor` or `Utility` instead.
*   **Benefit**: This is what makes our `native` tests so fast.
