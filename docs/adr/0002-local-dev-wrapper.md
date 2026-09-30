# ADR 0002: Local dev wrapper in `.local/NML_Dev` launched via `tools/wb-dev.ps1`

- **Status:** Accepted (2026-09-30, from Phase 0 spikes)

## Context

- The Enfusion MCP's `wb_launch(gprojPath)` copies its handler scripts into `<addon>/Scripts/WorkbenchGame/EnfusionMCP/`. Those must never reach a published NML addon.
- Phase 0 showed Workbench started with `-gproj <wrapper> -addonsDir <dir>` loads dependency addons from outside its profile folder. `wb_launch` cannot pass `-addonsDir`.
- Writing into a dependency addon from Workbench is not yet verified (MCP bridge lacks those calls in "game" mode) — a manual check is required before Phase 2 work relies on it.

## Decision

- A local-only wrapper addon `NML_Dev` at `<repo>/.local/NML_Dev/` (gitignored) depends on the base game and the NML addons. Handler scripts land there.
- `tools/wb-dev.ps1` starts Workbench with `-gproj .local\NML_Dev\NML_Dev.gproj -addonsDir addons` from the game folder; the MCP attaches afterwards.
- The Claude Code guard only allows `wb_launch` with the wrapper and `wb_cleanup` on NML/wrapper folders.
- Deviation from the original plan: the wrapper moved from `Documents\My Games\ArmaReforgerWorkbench\addons\NML_Dev` into the repo's gitignored `.local/`, so nothing NML-related is written to the protected Workbench profile folder.

## Fallback

If editing a dependency addon through the wrapper fails, open `addons/NML_Core/NML_Core.gproj` directly; handler scripts then land in a gitignored path and `tools/validate.mjs` blocks them from being committed/published.
