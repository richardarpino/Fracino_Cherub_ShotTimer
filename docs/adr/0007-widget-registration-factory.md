# ADR 0007: Widget Registry & Late-Binding Factory

## Status
Accepted (March 2026)

## Context
The `LVGLWidgetFactory` was previously dependent on the `WidgetType` enum and hardcoded template instantiations for every possible widget/sensor combination. This led to:
1. **Compile-time Bloat**: Every new sensor/widget combination required manual factory updates.
2. **UI Pollution in Native Labs**: The factory required LVGL headers to instantiate concrete widgets, breaking pure-logic native testing.
3. **Lack of Validation**: There was no mechanism to prevent invalid compositions (e.g., a "Service" tag being rendered in a "Gauge").

## Decision
1. **Introduction of WidgetRegistry**: Create a centralized registry (`IWidgetRegistry`) that stores `WidgetCompatibility` rules.
2. **String-Based Identity**: Replace the `WidgetType` enum with compile-time string constants (e.g., `GaugeWidgetTag::NAME`).
3. **Categorical Matching**: Use `DataCategory` (Telemetry/Service) and `PhysicalQuantity` as the primary filtering criteria for widget compatibility.
4. **Late-Binding Widgets**: Implement `Widget<void>` specializations that use string-based registry lookups at runtime instead of compile-time template arguments.
5. **MachineFactory Registration**: The `MachineFactory` now explicitly registers available widgets and their rules during system initialization.

## Consequences
- **Pros**:
    - **Decoupling**: The UI factory no longer needs to know about every sensor tag at compile-time.
    - **Native Logic Purity**: The `native` environment can now test orchestrators and dispatchers without including any UI-specific implementation or LVGL stubs.
    - **Automated Documentation**: The documentation gallery can use the registry to automatically filter out "nonsense" widget/sensor combinations.
    - **Extensibility**: Adding a new widget type only requires a one-line registration in `MachineFactory.cpp`.
- **Cons**:
    - **Initialization Sequencing**: The `WidgetRegistry` must be populated before any screens are drawn.
    - **String Lookups**: Minor runtime overhead for the initial compatibility check (mitigated by caching in the Painter).

## Alternatives Considered
- **Strongly-Typed Templates**: Keeping the template factory but adding more traits. Rejected because it didn't solve the UI-pollution in native tests.
- **Plugin System**: Full dynamic loading of widgets. Rejected as overkill for a embedded ESP32 system.
