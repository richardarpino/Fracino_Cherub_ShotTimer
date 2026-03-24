# Sensor Development Principles

The sensor framework in this project is designed to decouple "Hardware Acquisition" from "Logic Processing" while ensuring consistent data quality (smoothing/filtering) for all physical inputs.

## The Sensor Base Pattern

All physical hardware sensors MUST inherit from the **`HardwareSensor`** base class (or a specialized derivative like `DigitalSensor`).

### 1. Smoothing & Filtering (EMA)
Noisy hardware pins (ADC, HX711, etc.) must never feed raw data directly into the Logic Layer. The `HardwareSensor` base class provides a built-in Exponential Moving Average (EMA) filter.
- **Implementation**: Call `updateFilter(newRawValue)` in your sensor's polling logic.
- **Configuration**: Always provide an `alpha` parameter at the assembly point (MachineFactory). Use a low alpha (e.g., `0.1`) for heavy smoothing on jittery load cells or pressure sensors.

### 2. Hysteresis
To prevent UI flickering or "chatter" in state transitions, use the `getStableDisplayValue()` method. This ignores changes smaller than a defined `hysteresisThreshold`.

### 3. Separation of Concerns
- **IRawSource**: Low-level drivers that perform bit-banging or `analogRead()` (e.g., `HX711Source`). These should be "dumb" and focus only on bit-correctness.
- **HardwareSensor**: Middle-layer wrappers that add filtering, hysteresis, and unit metadata (e.g., `WeightSensor`).
- **ITagProcessor**: Logic-layer components that convert filtered raw data into final domain values (e.g., `WeightCalibrationProcessor`).

## Checklist for New Sensors
- [ ] Create/Use an `IRawSource` for the physical pin access.
- [ ] Wrap the source in a `HardwareSensor` derivative.
- [ ] **MANDATORY**: Initialize with a sensible `alpha` (smoothing factor) in `MachineFactory`.
- [ ] Publish the `FilteredValue` to the Registry, not the raw source value.
