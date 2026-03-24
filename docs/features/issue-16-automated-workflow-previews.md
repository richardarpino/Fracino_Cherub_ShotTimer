# Feature Planning: Automated Workflow Previews (Issue #16)
Status: Finished

## Problem Statement
Workflows and screen sequences are currently opaque to everyone except the production device user. This prevents rapid experimentation and makes it difficult to document the intended user journey for stakeholders.

## Goal
Establish an automated mechanism to "walk" through workflows in the simulator and generate a visual library (BMP + Markdown) that documents the full user experience, including screen flows and their corresponding triggers/blockers.

---

## Consensus Approach: Hybrid Centralized & Introspective
We combined **Workflow Factory Discovery** and **Introspective Metadata** for documentation richness.

### 1. Workflow Factory (Discovery)
A `WorkflowFactory` centralizes the assembly of all user journeys.
- Methods like `createStartupWorkflow()` provide named entry points.
- This allows the documentation tool to iterate over "all known workflows" easily.

### 2. Introspective Metadata (Richness)
Enhanced `IWorkflow` and `IScreen` to describe themselves:
- **Names**: Used for gallery indexing and folder naming.
- **Trigger Sentences**: Describe the *exit condition* for each screen (e.g., "Wait for WiFi connection").

### 3. Technical Implementation Details
- **Trigger Description**: Every `IScreen` implements `getExitCondition()` to provide a human-readable sentence for the documentation flow.
- **Sectioned Layout**: The documentation generator groups screens into "System" (WiFi/OTA/Warmup) and "Journeys" (Coffee/Maintenance) using a parent-child naming hierarchy.

---

## Technical Roadmap

### Iteration 1: Infrastructure & Metadata
- **Goal**: Enable introspection and centralized discovery.
- **Changes**: 
    - Update `IWorkflow`/`IScreen` with Name/Description/ExitCondition.
    - Implement `WorkflowFactory` for centralized assembly.
- **Result**: `WorkflowFactory` implemented; metadata support verified via native tests.

### Iteration 2: Automated Snapshotting
- **Goal**: Generate pixel-perfect BMPs for all screens in a workflow.
- **Changes**: 
    - Create `WorkflowSnapshotter` in simulator test suite.
    - Implement automatic traversal of `WorkflowFactory` outputs.
- **Result**: BMP files generated in `docs/previews/`. Resolved simulator environment regressions (resolution and parenting) to ensure fidelity.

### Iteration 3: Markdown Documentation
- **Goal**: Generate navigable "Machine Journeys" galleries.
- **Changes**: 
    - Implement Markdown emitter in simulator.
    - Combine BMP links with trigger descriptions in a flow table.
- **Result**: Navigable READMEs generated in `docs/previews/` combining images and exit conditions.

### Iteration 4: Finalization
- **Goal**: Polish and architectural audit.
- **Result**: Integrated with verification workflows; final architecture audit completed.

---

## Design Decisions & ADRs
- **ADR-0008**: Hybrid Metadata/Factory Approach for Documentation (derived from this planning).
- **Simulator Fidelity**: Chose to maintain a strictly hardware-aligned (240x135) rendering context in the simulator to ensure accuracy.
