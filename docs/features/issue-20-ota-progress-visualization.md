# Feature Planning: OTA Download Progress Visualization (Issue #20)
Status: Finished

## 1. Problem Statement
The current OTA firmware update process occurs silently in the background. When an update is received, the board restarts without warning, which can appear as a system glitch or a crash to the user, especially if they are actively using the machine.

## 2. Goal
Leverage the **Global Trigger** system (introduced in Issue #19) to provide immediate visual feedback when an OTA download begins. This involves showing a dedicated OTA screen with a real-time progress bar that interrupts any active workflow.

### Core Requirements:
1.  **Global Intercept**: The OTA progress screen must appear regardless of the current active workflow (e.g., interrupting the Dashboard or a Shot).
2.  **Real-time Progress Mapping**: The screen must display a percentage progress bar derived from the `OTAService` download status.
3.  **Error Handling**: If the update fails, the screen should transition to an error state or provide clear feedback before reverting to the previous workflow.

---

## 3. Proposed Architecture

### A. The `OTADownloadingTrigger`
A new implementation of the `ITrigger` interface will watch the `OTAStatus` tag in the `SensorRegistry`.
- **Condition**: Returns `true` if `0.0f <= progress < 100.0f` AND `isFailed == false`.
- **Note**: This ensures the screen pops up as soon as the first byte is received and stays active until completion or failure.

### B. Global Registration
The `WorkflowEngine` will have the `OTAUpdateWorkflow` registered as a Global Trigger in `MachineFactory`.
- Since it's global, it bypasses the normal tree traversal and takes priority over the root's current active branch.

---

### C. The Heartbeat Mechanism
To ensure the UI remains responsive during blocking OTA downloads (caused by `ArduinoOTA.handle()`), the `OTAService` provides a `heartbeat` callback.
- **Implementation**: The `main.cpp` loop logic (Painter/Workflow) is injected into the OTA progress callback.
- **Throttling**: To maintain high download speeds, the heartbeat and registry updates are throttled to **5Hz (200ms)**.

---

## 4. Final Implementation & Adjustments

### Heartbeat Injection
The `MachineFactory` facilitates the delivery of the `main.cpp` heartbeat to the `OTAService`, ensuring the `WorkflowEngine` and `LVGLPainter` continue to refresh the progress bar even when the main loop is blocked.

### Succession Pattern Refactor
During the fix for the "Warming Up" regression, we moved from an `AlwaysTrigger` child pattern to a `DefaultWorkflow` fallback. This ensures the full `System Startup` sequence completes before the `Dashboard` takes over.

---

## 5. Verification Results

### Native Tests
- **test_ota_global_interrupt**: PASSED.
- **test_ota_screen_skip_bug**: PASSED (Verifies sync between OTA service and engine).
- **test_workflow_hierarchy_skip_bug**: PASSED (Verifies sequential screen advance).

### Simulator & Hardware
- **Visual Progress**: Verified in simulator.
- **Production Hardware**: Performance tested; smooth 0-100% progress bar at 5Hz.
