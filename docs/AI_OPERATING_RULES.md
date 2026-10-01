# NML AI Operating Rules

These rules apply to every AI-assisted session (Claude, Codex or any other tool) working on NML. They sit alongside [AI_WORKBOARD.md](AI_WORKBOARD.md), which is the current-task and ownership source of truth. Do not infer approval: if something is not explicitly allowed here or by Weston, ask.

## 1. Project state

- NML is an open-world faction combat sandbox.
- No objectives, capture points, score, rounds, win condition, forced lanes or scripted progression.
- **Stage 5.3 is blocked until Weston explicitly approves it.**
- The V1 Workshop mod list and the V1 arsenal are still being finalized.

## 2. Authority

Weston is the project lead. Only Weston may approve:

- architecture changes
- NML_Core design changes
- dependency promotion
- Workshop mods becoming production dependencies
- accepted ADR changes
- Stage 5.3
- production server configuration changes
- final merges of Cornelius/FRIT contributor work, unless explicitly delegated

## 3. Required pre-task checks

Before any NML task, the AI must:

1. Read `docs/AI_OPERATING_RULES.md` (this file).
2. Read `docs/AI_WORKBOARD.md`.
3. Check current `main`.
4. Check relevant open PRs and issues.
5. Confirm the task owner.
6. Confirm whether the work is research, testing or implementation.
7. Confirm the allowed files and subsystems.
8. Stop if there is overlap with another active task.

## 4. Hard-stop conditions

STOP and ask Weston before:

- modifying `addons/NML_Core/`
- modifying `dependencies/`
- modifying `server/`
- modifying `docs/adr/`
- modifying `.github/`
- modifying shared tooling
- changing any `.gproj`
- adding or removing Workshop dependencies
- changing production server mod lists
- implementing Stage 5.3
- changing architecture or accepted ADR decisions
- touching another contributor's active task or branch
- vendoring or modifying third-party Workshop content

Do not infer approval.

## 5. Third-party mod rules

- Workshop mods are external dependencies.
- Do not vendor or copy them into NML.
- Do not modify third-party source or assets unless explicitly approved and the license permits it.
- Record the Workshop ID, project GUID, version, dependencies and relevant resources.
- Research or testing does not equal approval.

## 6. Git rules

- Never work directly on `main`.
- Never force-push `main`.
- Never bypass branch protection.
- One branch per task.
- Do not modify another contributor's branch.
- Cornelius and FRIT do not merge their own PRs unless Weston explicitly authorizes it.
- Always report the exact files changed.

## 7. Research rules

Research and testing may:

- inspect Workshop mods
- inspect `.gproj` files
- discover GUIDs
- discover direct and transitive dependencies
- discover prefab and resource paths
- test compatibility
- document findings

Research does **not** authorize production changes.

## 8. Conflict rule

If overlap or conflict is detected, STOP and report:

- the overlap
- the owner
- the files or subsystem involved
- the recommended next action

Do not resolve the conflict automatically.

## 9. Completion rule

At the end of every task, report:

- branch
- files changed
- tests run
- validation result
- dependencies affected
- overlap/conflict status
- what still needs approval

Then STOP.
