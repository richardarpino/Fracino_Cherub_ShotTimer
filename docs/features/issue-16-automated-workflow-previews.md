# Feature Planning: Automated Workflow Previews (Issue #16)

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

### 3. Structured Documentation
Documentation is organized into:
- **System section**: WiFi, OTA, Warmup.
- **Core Journey sections**: Coffee Journey, Maintenance.
- **Hierarchical Flow**: Using parent-child naming to imply state progression.

---

## Technical Roadmap

### Iteration 1: Infrastructure & Metadata
- **Objective**: Enable introspection and centralized discovery.
- **Result**: `WorkflowFactory` implemented; `IWorkflow` and `IScreen` updated with metadata support.

### Iteration 2: Automated Snapshotting
- **Objective**: Generate BMPs for all screens in a workflow.
- **Result**: `WorkflowSnapshotter` in simulator traverses all factory-generated workflows. Resolved simulator environment regressions (resolution and parenting) to ensure pixel-perfect fidelity.

### Iteration 3: Markdown Documentation
- **Objective**: Generate navigable "Machine Journeys" READMEs.
- **Implementation**: `MarkdownGenerator` utility in simulator combines BMPs with exit conditions into structured tables.

### Iteration 4: Finalization
- **Objective**: Polish and architectural audit.

---

## Design Decisions & ADRs
- **ADR-0008**: Hybrid Metadata/Factory Approach for Documentation (derived from this planning).
- **Simulator Fidelity**: Chose to maintain a strictly hardware-aligned (240x135) rendering context in the simulator to ensure accuracy.
