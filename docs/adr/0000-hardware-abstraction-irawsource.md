# ADR 0000: Hardware Abstraction via IRawSource

*   **Status**: Accepted
*   **Date**: 2026-01-22 (Project Inception)

## Context
At the start of the project, we needed to develop sensor logic (e.g., pressure-to-bar conversion, weight auto-taring) before the physical sensors or even the final wiring were confirmed. 
1.  **Hardware Dependency**: Direct calls to `analogRead()` or `digitalRead()` make code impossible to test in a `native` environment.
2.  **Unpredictable Noise**: Physical ADC inputs are notoriously noisy and require averaging before they can be used for telemetry.
3.  **Parallel Development**: We needed to build the Shot Timer features before the pressure sensor hardware arrived.

## Decision
We introduced the **`IRawSource` interface** to separate "Data Acquisition" from "Data Representation":
1.  **Interface Segregation**: A `Sensor` (e.g., `BoilerPressure`) does not read a pin. It reads from an `IRawSource`.
2.  **Hardware Implementation**: `ADCRawSource` handles the physical `analogRead()` and implements 64-sample averaging.
3.  **Fake Implementation**: `MockRawSource` (and later `SensorStub`) allows us to feed explicit values into the system during testing.

## Consequences
*   **Positive**: **Zero-Hardware Development**. We were able to write and verify the entire pressure and weight logic without a single sensor connected.
*   **Positive**: Enforces a strict boundary between "What the voltage is" and "What the pressure is."
*   **Positive**: Allows for easy addition of new hardware (like moving from an internal ADC to an external I2C ADC) without changing the `BoilerPressure` logic.
