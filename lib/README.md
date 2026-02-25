# 📦 Internal Library Structure

This directory contains the core machine logic, hardware abstractions, and UI components. We follow a **Pure Factory** and **Coordinator-based** architecture.

## 📂 Directory Guide

### 🏭 `Factories/`
The "Assembly Line" of the machine.
- **`MachineFactory`**: The single source of truth for Dependency Injection. Wires concrete hardware to abstract logic.

### 🔌 `Interfaces/`
The "Contract Layer." All logic depends on these abstractions to remain host-independent.
- **`ISensor` / `ISwitch`**: Telemetry and state protocols.
- **`IBlocker`**: Transient startup gate protocol (WiFi, Warmup).
- **`IRawSource`**: Basic pin/ADC abstraction.

### 🧠 `Logic/`
The orchestrators (Coordinators). Process-specific behavior that combines multiple data sources and handles UI state logic.
- **`StartupLogic`**: Manages the boot journey via `IBlocker` states.
- **`ScaleLogic`**: High-level espresso weighing and timing logic.
- **`ThemeManager`**: Handles display theme switching based on `ISwitch` trigger events.

### 🌡️ `Sensors/`
Data acquisition components.
- **`Hardware/`**: Concrete sensors like `BoilerPressure` and `WeightSensor`.
- **`Virtual/`**: Logic-based decorators like `DebouncedSwitch` or `EMAFilter`.

### 🛰️ `Services/`
System-level lifecycle managers and transient startup gates.
- **`WiFiService`**: Manages connection and network status.
- **`OTAService`**: Handles wireless firmware updates.
- **`WarmingUpBlocker`**: Detects boiler warmup cycles via dimensional history.

### 🔌 `Hardware/`
The lowest level of the stack. Direct pin scanners (`ADCRawSource`, `DigitalRawSource`).

### 🎨 `Themes/` & `UI/`
The presentation layer. Passive widgets that consume data from the logic layer.

---

## 🛠 Developer Workflow

1.  **Architecture First**: Read the **[📜 Developer Standards & Principles](../docs/README.md)** before starting.
2.  **TDD**: Start with a `native` test using stubs.
3.  **Abstractions**: Use `lib/Interfaces`. Do not leak hardware-specific code into `lib/Logic`.
4.  **Wiring**: Update the `MachineFactory` to register new components.
