# Architectural Pillars

These principles provide **Guidance** rather than rigid Rules. The goal is structural integrity, but we value **Evolution** over perfection. It is often better to put something in the "wrong" place to see how it behaves and then refactor later once the pattern emerges.

## 0. Rule Zero: Pragmatic Evolution
**The Guidance**: Abstractions are discovered, not manufactured.

*   **Temporary Coupling**: It is okay to couple components or place logic in suboptimal locations during feature exploration.
*   **Divergence points to Discovery**: When the code feels "messy" or "wrong," it is a signal that a new abstraction is trying to emerge. This is when we refactor into the patterns below.
*   **Refinement over Correctness**: We prioritize working code that can be easily split apart later over complex abstractions built too early.

## 1. Domain-Driven Design (DDD) & Ubiquitous Language
**The Rule**: We use a shared, ubiquitous language derived from the espresso machine domain (`WarmingUp`, `ShotTimer`, `PumpSignal`).

*   **Intent-Revealing Names**: Class and variable names must describe *intent*, not just "how" or "what" type they are (e.g., `WarmingUpBlocker` vs. `CycleCounter`).
*   **Bounded Contexts**: The `Logic/` layer should be shielded from the "noise" of hardware. A `ShotTimer` doesn't care about "Pin 25"; it cares about a `PumpSignal` switch being active.

## 2. SOLID: Single Responsibility (SRP)
**The Rule**: Every class must have one, and only one, reason to change.

*   **Hardware Sensors**: Passive observers of physical signals. They inherit from `HardwareSensor`, process raw values, and apply EMA filtering. They **never** contain machine logic.
*   **Decouplers**: `IRawSource` separates the sensor's physical measurement from its logical representation.
*   **Logic Components**: Derived calculators (like `BoilerTemperature`) or state orchestrators (like `ShotTimer`). They live in `lib/Logic`, use `HardwareSensor` values as inputs, and **publish** results to the `ISensorRegistry`.
*   **Blockers**: Manage a specific startup gate.

## 3. Dependency Injection (DI) & The Factory
**The Rule**: High-level logic modules MUST NOT instantiate their own dependencies. 

*   **MachineFactory**: This is the single source of truth for Dependency Injection. It wires the concrete hardware to the abstract logic.
*   **Testable DI**: By injecting dependencies (like `ISensor*`) into constructors, we enable the `native` test suite to swap real hardware for stubs (`SensorStub`) instantly.

## 4. Coordinator-Logic Pattern (Registry Producers)
**The Rule**: Sensors and Switches are passive data pipes. Behavior logic must reside in `Logic/` orchestrators which act as **Registry Producers**.

*   **The Pattern**: Instead of components being "polled" by the UI, logic modules coordinate hardware and **push** high-level machine data (`ShotTimer`, `TaredWeight`) into the `ISensorRegistry`.
*   **Type Tags as Domain Contracts**: Tags (e.g., `BoilerPressureTag`) are not just markers. They contain static `SensorMetadata` that defines the measurement limits, units, and nominal values. This ensures the Registry and UI always have a consistent "Contract" for the data.
*   **Benefit**: The UI remains completely decoupled. It pulls from the Registry by a `TypeTag`, oblivious to whether the data came from a physical pin or a complex logical calculation.
*   **Verification**: This enables 100% test coverage in `native` by verifying that the correct data is "deposited" into the Registry during the update loop.

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
