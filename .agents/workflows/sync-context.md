---
description: Re-establish context cleanly to ensure AI and Human are aligned.
---
# The `/sync-context` Protocol

This workflow MUST be run after a crash, a long break, or whenever the AI seems "lost" or "hallucinating" (e.g., printing kanji).

1. **Verify Baseline**: 
    - Run `/verify-all` to ensure the current state is stable.
2. **Environmental Audit**:
    - Run `git status` to see actual code changes.
    - Grep for `TODO` or `FIXME` in the modified files.
3. **Documentation Sync**:
    - Read the current `task.md` and `implementation_plan.md` in the artifact directory.
    - Check for any `pivot_from_*.md` notes or `*_PAUSED.md` implementation plans.
    - Read the most recent post-mortem in `docs/post_mortems/`.
4. **Alignment Pitch**:
    - The AI must provide a 3-sentence summary:
        - "Where we were level-wise (e.g., Phase 3: Stabilization)."
        - "What the last successful verification was."
        - "What the very next action is."
5. **Confirmation**:
    - Wait for the user to say "Proceed" or provide a correction.
