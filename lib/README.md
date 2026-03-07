# 📦 Internal Library Structure

This directory contains the core machine logic, hardware abstractions, and UI components. We follow a **Pure Factory** and **Coordinator-based** architecture.

## 📂 Core Architecture: The Reactive Chain

This project uses a **Type-Safety Registry** to decouple hardware from logic. The system is a directed acyclic graph (DAG) of data dependencies.

### 1. Data Definitions (`Interfaces/SensorTags.h`)
Type-Tags define the "Domain Contract." They specify units, limits, and **Dependencies**:
- **Tag Inheritance**: A tag like `BoilerPressureReading` can define `Children = TagList<HeatingCycleReading>`.
- **Compile-Time Safety**: This ensures the registry knows exactly which processors to trigger when a parent value changes.

### 2. The Registry (`Logic/SensorDispatcher`)
The single source of truth for the machine state.
- **`publish<T>(value)`**: When hardware (or a processor) publishes a value, the Registry immediately identifies dependent tags and triggers their attached processors.
- **Host Independence**: The registry works identically on the ESP32 and in `native` unit tests.

### 3. Processors (`Logic/Processors/`)
Passive logic components that transform telemetry.
- **Registration**: Processors are attached to their output tags using `registry.attachProcessor<T>(processor)`.
- **Execution**: They do not have their own timers; they are called by the Registry the moment their input data is ready.

### 4. Wiring it Together (`Factories/MachineFactory.cpp`)
The Factory is the "Wiring Diagram":
1.  **Provide Sensors**: `registry.provide<BoilerPressureReading>(&pressureSensor)`.
2.  **Attach Processors**: `registry.attachProcessor<HeatingCycleReading>(&heatingCycleProc)`.

---

## 🛠 Developer Workflow

1.  **Architecture First**: Component logic belongs in `lib/Logic/Processors`.
2.  **TDD**: Start with a `native` test by publishing raw values to the registry and asserting the derived results.
3.  **No Hardware Leakage**: If you find yourself using `digitalRead()` or `millis()` in a Processor, you are breaking the abstraction. Use `RegistrySwitch` or `SystemUptimeReading` instead.
