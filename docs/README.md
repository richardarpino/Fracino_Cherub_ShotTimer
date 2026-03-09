# Project Standards & Documentation

Welcome to the internal standards of the Fracino Cherub Shot Timer project. This directory contains the "Laws of the Codebase"—the principles and workflows that ensure this project remains maintainable, testable, and friendly to both humans and AI agents.

## 🧭 Navigation

### 🏗️ [Architectural Principles](principles.md)
The core pillars of the system design. Understand the "Why" behind the "How."
*   Coordinator-Logic Pattern
*   Passive UI Strategy
*   Hardware Abstraction (IMachineProvider)

### ☕ [Project Philosophy](philosophy.md)
The heart and soul of the project. Why we do this and what we believe in.
*   Passion for Espresso
*   Learning through Iteration
*   Sharing & Openness

### 🛠️ [Development Workflow](workflows.md)
The mandatory processes for contributing to this project.
*   The TDD Charter
*   Environment Selection (`native` vs `simulator` vs `hardware`)

### 🤖 [Agentic Rules of Engagement](ai-agents.md)
Specific instructions and requirements for AI Agentic systems contributing to this repository.
*   PR Approval Criteria
*   Context Retrieval Requirements
*   Verification Standards

### 📜 [Architecture Decision Records (ADR)](adr/)
The historical log of major technical decisions.
*   [ADR 0000: Hardware Abstraction via IRawSource](adr/0000-hardware-abstraction-irawsource.md)
*   [ADR 0001: Coordinator-Logic Pattern](adr/0001-coordinator-logic-pattern.md)
*   [ADR 0002: Native-First Verification](adr/0002-native-first-verification.md)
*   [ADR 0003: ISensor vs IBlocker Segregation](adr/0003-isensor-vs-iblocker-segregation.md)
*   [ADR 0007: Widget Registry & Late-Binding Factory](adr/0007-widget-registration-factory.md)

### 📈 [Feature Design History](features/)
Detailed planning documents for major feature shifts.
*   [Issue #15: Widget Registry & Late-Binding Architecture](features/issue_15_widget_registry.md)

---

*“Documentation is a love letter that you write to your future self.”* — or in this case, to the next Agent that takes the wheel.
