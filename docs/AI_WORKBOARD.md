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
- [x] Stage 5.4 two-client validation and docs done; PR awaiting Weston's review
- [ ] Stage 5 close-out: Stage 5.4 PR merged
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
| Stage 5.4 Arsenal validation + docs | Weston | `test/stage-5.4-two-client-validation` | NML_Tests helper, architecture/capability docs | In review |
| Myrove evaluation | TBD | — | Workshop dependency research | Planned |
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
