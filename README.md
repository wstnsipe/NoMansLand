# No Man's Land (NML)

A hardcore multiplayer Arma Reforger server and mod project.

> Status: local DEV loop established (Phase 4): four NML addons, a DEV scenario on Everon, local dedicated server + clients, first Autotest suite. No gameplay systems yet. Daily loop: [docs/workflows.md](docs/workflows.md).

## Layout

| Path | What |
|---|---|
| `addons/` | NML Enfusion addons (`NML_Core`, `NML_Content`, `NML_Scenario_Dev`, `NML_Tests`) — created in Phase 2 |
| `dependencies/` | Third-party mod registry (`mods.json`), candidate backlog, capability map |
| `server/` | Server config templates for DEV / TEST / LIVE (no secrets) |
| `tools/` | Validator, Workbench dev launcher, git hooks, tests |
| `docs/` | Architecture, workflows, release process, ADRs |
| `.claude/` | Shared Claude Code safeguards (deny rules + NML guard hook) |

## Quick start

1. Install Git LFS, Node 18+, Arma Reforger Tools (Steam).
2. `powershell -ExecutionPolicy Bypass -File tools\install-hooks.ps1`
3. `node tools/validate.mjs`
4. Read [CONTRIBUTING.md](CONTRIBUTING.md) and [docs/workflows.md](docs/workflows.md).

## Key rules

- Creator tag: **`NML_`** on every class, enum, file and addon.
- Server is authoritative; all scripts ship to clients — never put secrets in the mod.
- Third-party mods are external dependencies only: never vendored or modified. See [dependencies/README.md](dependencies/README.md).
- `.meta` files are committed and always travel with their resource.

Public, source-visible repository. All rights reserved: no license is granted (interim terms, to be finalized). See [LICENSE](LICENSE).
