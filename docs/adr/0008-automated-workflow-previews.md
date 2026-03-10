# ADR 0008: Automated Workflow Mapping & Visual Documentation

## Status
Accepted

## Context
As the codebase grew, understanding the full user journey (WiFi -> OTA -> Warmup -> Dashboard -> Shot) required a physical device. This made it difficult for developers and AI agents to verify UI changes without hardware. We needed a way to automatically "walk" through workflows and generate visual documentation.

## Decision
We implemented a **Hybrid Metadata/Factory Documentation** system:

1.  **Workflow Discovery**: A centralized `WorkflowFactory` provides a list of all known machine journeys.
2.  **Introspective Metadata**: Every `IScreen` provides its `Name`, `Description`, and a human-readable `ExitCondition`.
3.  **Simulator Snapshotting**: A `WorkflowSnapshotter` utility in the simulator test suite takes BMP screenshots as it traverses each workflow.
4.  **Markdown Generation**: A `MarkdownGenerator` utility transforms the captured images and metadata into structured, navigable galleries.
5.  **Fidelity Standard**: The simulator is strictly locked to the hardware's native portrait resolution (240x135) to ensure snapshots accurately reflect the device.

## Consequences
- **Positive**: Complete user journeys are now self-documenting and verifiable in CI.
- **Positive**: Improved discoverability of screen logic and exit conditions.
- **Negative**: Adds minor runtime overhead to the simulator test suite (though negligible compared to build time).
- **Negative**: Requires developers to keep `getExitCondition()` and `WorkflowFactory` updated as new screens/journeys are added.
