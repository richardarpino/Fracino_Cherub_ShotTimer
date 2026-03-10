# Feature Plan: Widget Registry & Categorical Mapping (Issue #15)

## Status
Developed (March 2026)

## Context
As the project scales its UI layout (1x1, 1x2, 2x2 grids), the number of specialized widgets (Gauges, Sensors, Shot Timers) has increased. Previously, the `LVGLWidgetFactory` used hardcoded string checks (`strcmp`) to map registry tags to widget classes. This was brittle and led to "Visual Drift" in the simulator gallery.

## Architectural Decision: WidgetRegistry
We have introduced a `WidgetRegistry` as the central authority for UI mapping. This registry mirrors the `SensorRegistry` (dispatcher) and provides a "late binding" mechanism for widgets.

### 1. Data-Categorical Mapping
Instead of mapping widgets to specific sensors, we map them to **Data Categories**:
- `TELEMETRY`: Continuous numeric readings (Pressure, Temperature, Weight).
- `SERVICE`: Discrete system status messages (WiFi, OTA, Startup).

### 2. Matching Precedence
The `WidgetRegistry::isCompatible(widgetName, tagName)` uses the following logic:
1. **Specific Match**: If a widget explicitly registers a compatibility with a `specificTag` (e.g., `ShotTimerWidget` claims `ShotTimeReading::NAME`).
2. **Wildcard Categorical Match**: If a widget (like `SensorWidget`) has no specific quantity constraints, it acts as a generic fall back for its entire category (`TELEMETRY`).
3. **Quantity Matching**: Specialized widgets (like `GaugeWidget`) only match if the tag's `PhysicalQuantity` (e.g., `PRESSURE`) is in the widget's allowed list.

### 3. Late-Binding Factory
The `LVGLWidgetFactory` no longer uses template instantiations for every sensor type. Instead:
1. It queries the `WidgetRegistry` for compatibility.
2. It instantiates the widget using the `Widget<void>` specialization.
3. The widget instance fetches its own metadata and data from the `ISensorRegistry` at runtime using its assigned `tagName`.

## Implementation History
- **Iteration 1**: Created `IWidgetRegistry` and `WidgetRegistry` infrastructure.
- **Iteration 2**: Refactored `MachineFactory` and `LVGLWidgetFactory` to use string-based registration.
- **Iteration 3**: Integrated the registry into the simulator gallery (`test_simulator`) to filter out incompatible visual variants.
- **Iteration 4**: Purged all legacy `WidgetType` enum references from the codebase.
