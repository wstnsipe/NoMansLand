# NML AI Workboard

All AI-assisted work must also follow [docs/AI_OPERATING_RULES.md](AI_OPERATING_RULES.md).

## Global Rules

- `main` is protected. Never work directly on `main`.
- Every task gets its own branch and PR.
- Check this file before starting any NML work.
- Check open PRs/issues before editing a subsystem.
- Do not duplicate another contributor's active task.
- If planned work overlaps another contributor's scope, STOP and report it.
- Third-party Workshop mods remain external dependencies.
- Do not vendor or modify third-party mods.
- Dependency changes require lead approval.
- Architecture changes require lead approval/ADR.
- Each phase stage is a gate: the next stage starts only with explicit lead approval.
- GitHub `main`, accepted ADRs, and this workboard are the source of truth.

---

## Weston / Claude

### Primary responsibilities
- NML architecture
- NML_Core
- dependency approval/integration
- server configuration
- accepted ADR implementation
- final integration
- release workflow

### May modify
- `addons/NML_Core/`
- `server/`
- `dependencies/`
- architecture docs
- shared tooling when required

### Do not
- overwrite active Cornelius/FRIT research
- approve mods automatically
- merge another contributor's work without review

### Current
- [x] Phase 5.2 complete
- [x] Stage 5.3 complete (PR #7, `568f363`)
- [x] Stage 5.4 complete (PR #8, `b4ee89f`); Stage 5 closed
- [ ] Stage 6.1: proposed ADR 0006 (Myrove), Mangrove → Myrove naming, workboard cleanup; PR awaiting Weston's review
- [ ] Stage 6.2–6.7: Myrove candidates, dependency tooling, registration, `NML_Scenario_Myrove`, compatibility strategy, arsenal handoff contract (each a separate gate)
- [ ] Finalize V1 dependency set
- [ ] Finalize V1 arsenal inputs

---

## Cornelius / Codex

### Primary responsibilities
- Workshop mod evaluation
- arsenal research
- resource/prefab discovery
- faction/loadout research
- compatibility testing
- dependency discovery

### Allowed
- read NML architecture and configs
- inspect downloaded Workshop `.gproj` files
- discover GUIDs/dependencies
- run Workbench and validation
- create scoped research/test branches when assigned

### Do not without approval
- change NML_Core architecture
- add approved dependencies to production configs
- modify `main`
- implement Stage 5.3
- vendor third-party mods

### Current tasks
- [ ] MOD-___:
- [ ] ARS-___:

### Files/subsystems currently owned
- None

---

## FRIT / Codex

### Primary responsibilities
- Workshop mod evaluation
- arsenal research
- resource/prefab discovery
- faction/loadout research
- compatibility testing

### Allowed
- same research/test capabilities as Cornelius

### Do not without approval
- change NML_Core architecture
- modify production dependencies
- implement Stage 5.3
- touch another contributor's active task files

### Current tasks
- [ ] MOD-___:
- [ ] ARS-___:

### Files/subsystems currently owned
- None

---

## Active Task Lock Table

| Task | Owner | Branch | Scope | Status |
|---|---|---|---|---|
| Stage 5.3 Arsenal implementation | Weston | `feat/arsenal-curation` | NML_Core arsenal system | Merged (PR #7) |
| Stage 5.4 Arsenal validation + docs | Weston | `test/stage-5.4-two-client-validation` | NML_Tests helper, architecture/capability docs | Merged (PR #8) |
| Myrove evaluation | Weston | — | Feasibility spike (Workbench, dedicated server, client) | Done; findings in `docs/research/2026-10-02-arsenal-status.md` |
| Stage 6.1 Myrove ADR + naming + workboard | Weston | `chore/stage-6.1-myrove-adr` | Docs only: ADR 0006 (proposed), Myrove naming | In review |
| Stage 6.2–6.5 Myrove integration | Weston | per sub-stage | Candidates, tooling, registration, `NML_Scenario_Myrove` | Planned; each needs approval |
| V1 arsenal research | Cornelius/FRIT split | — | Resource discovery only | Planned |

---

## Before Starting Any Task

- [ ] Pull latest `main`
- [ ] Read this file
- [ ] Check open PRs/issues
- [ ] Confirm task owner
- [ ] Confirm branch name
- [ ] Confirm files/subsystems in scope
- [ ] Confirm no overlap
- [ ] Confirm whether task is research-only or implementation
- [ ] Confirm approvals required

## Before Opening a PR

- [ ] Rebase/update from current `main`
- [ ] Run required validation
- [ ] List exact files changed
- [ ] List dependencies affected
- [ ] State whether another contributor's work is impacted
- [ ] Update this workboard if ownership/status changed
