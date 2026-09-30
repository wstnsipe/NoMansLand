# ADR 0002: Local dev wrapper in `.local/NML_Dev` launched via `tools/wb-dev.ps1`

- **Status:** Accepted (2026-09-30, from Phase 0 spikes)

## Context

- The Enfusion MCP's `wb_launch(gprojPath)` copies its handler scripts into `<addon>/Scripts/WorkbenchGame/EnfusionMCP/`. Those must never reach a published NML addon.
- Phase 0 showed Workbench started with `-gproj <wrapper> -addonsDir <dir>` loads dependency addons from outside its profile folder. `wb_launch` cannot pass `-addonsDir`.
- Writing into a dependency addon from Workbench was verified manually on 2026-09-30 (see Verification below). The MCP bridge lacks those calls in "game" mode, so the check was done in the Resource Browser.

## Decision

- A local-only wrapper addon `NML_Dev` at `<repo>/.local/NML_Dev/` (gitignored) depends on the base game, `NML_Core`, `NML_Content`, `NML_Scenario_Dev` and `NML_Tests` by GUID. Each developer creates it locally (template in `docs/workflows.md`); its own GUID is local and never referenced. Handler scripts land there.
- `tools/wb-dev.ps1` starts Workbench with `-gproj .local\NML_Dev\NML_Dev.gproj -addonsDir addons` from the game folder; the MCP attaches afterwards.
- The Claude Code guard only allows `wb_launch` with the wrapper and `wb_cleanup` on NML/wrapper folders.
- Deviation from the original plan: the wrapper moved from `Documents\My Games\ArmaReforgerWorkbench\addons\NML_Dev` into the repo's gitignored `.local/`, so nothing NML-related is written to the protected Workbench profile folder.

## Verification (2026-09-30, manual, Resource Browser)

Workbench was started with the same command line as `tools/wb-dev.ps1` (`-gproj <wrapper> -addonsDir <dir>`) against a throwaway wrapper and dependency addon under `.local/`:

- Workbench loaded the dependency addon through `-addonsDir`.
- **Register** created the resource's `.meta` in the real dependency folder.
- **Edit + save** (Config Editor) wrote directly to the dependency's file.
- **Duplicate** created a new file and a unique `.meta` in the dependency folder.
- No stray copies appeared in the wrapper or the Workbench profile folder.

## Fallback (not needed)

Because the verification passed, the fallback is not used. It stays documented only in case a future Workbench update breaks dependency editing: open `addons/NML_Core/NML_Core.gproj` directly; handler scripts then land in a gitignored path and `tools/validate.mjs` blocks them from being committed/published.
