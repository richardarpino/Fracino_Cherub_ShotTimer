# ADR 0012: UI Heartbeat for Blocking OTA Services

## Status
Accepted

## Context
The `ArduinoOTA` library's `handle()` method is a blocking call that consumes significant CPU time during firmware downloads. Because our main application loop (in `main.cpp`) relies on frequent calls to `engine.update()` and `painter.paint()` to refresh the UI, the OTA download would previously "freeze" the display until completion.

For Issue #20 (Progress Visualization), a frozen screen is unacceptable as the user needs to see the progress bar moving to stay informed.

## Decision
We will implement a "Heartbeat" callback mechanism to inject critical UI and logic updates into the blocking service's internal loop.

- **Callback Injection**: `OTAService` accepts a `std::function<void()>` heartbeat. This callback is executed inside the `onProgress` handler of the OTA download.
- **Propagation**: `MachineFactory` provides the bridge to pass the `main.cpp` logic into the `OTAService`.
- **Throttling**: The heartbeat is throttled to 5Hz (200ms) to prevent excessive CPU contention and ensure the firmware download speed remains high.

## Consequences
- **Responsive UI**: The progress bar remains smooth and interactive even during high-bandwidth downloads.
- **Workflow Integrity**: The `WorkflowEngine` continues to evaluate triggers, allowing the OTA screen to correctly transition out upon completion or error.
- **Complexity**: Adds a slight coupling between the main loop and the service layer via the callback, but preserves the overall architecture by injecting logic rather than duplicating it.
