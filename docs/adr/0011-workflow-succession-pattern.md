# ADR 0011: Workflow Succession Pattern (Sibling Fall-Through)

## Status
Accepted

## Context
The `WorkflowEngine` (Issue #19) supports a hierarchical tree of triggers. We needed a reliable way to transition from the "Boot Sequence" (Startup) to the "Main App" (Dashboard) without skipping intermediate screens (WiFi -> OTA -> Warming Up).

Our first attempt used `DefaultWorkflow`, but this bypassed the trigger tree entirely, making contextual triggers like the `Shot Timer` (pump-driven) unreachable while in the idle state.

## Decision
We will use a **"Sibling Fall-Through"** pattern to handle succession between primary system states.

- **Precedence-Based Priority**: Primary sequences (e.g., `Startup`) are added as high-priority siblings (Prio 10) under the system root. Successors (e.g., `Dashboard`) are added as low-priority siblings (Prio 1).
- **Engine Logic**: `WorkflowEngine::findNextActiveWorkflow` is enhanced to skip any candidate node if its associated workflow reports `isFinished() == true`.
- **Fall-Through**: This allows the engine to naturally "fall" to the next available sibling in precedence order once the previous one is done.

## Consequences
- **Tree Integrity**: The `Dashboard` remains a full node in the trigger tree, allowing its contextual children (like `Shot`) to be evaluated and triggered correctly.
- **Sequential Safety**: Multi-screen sequences are guaranteed to finish before the engine falls through to the next peer.
- **Explicit Hierarchy**: Clear distinction between Succession (Siblings), Interruption (High-Prio Global Triggers), and Contextual Branching (Children).
