# Workflows

## Daily loop (DEV)

1. `git switch main && git pull`, then `git switch -c feat/<system>-<desc>`.
2. Start Workbench on the local wrapper: `powershell -ExecutionPolicy Bypass -File tools\wb-dev.ps1`.
   - It launches Workbench with `-gproj .local\NML_Dev\NML_Dev.gproj -addonsDir addons` so the NML addons resolve from your checkout ([ADR 0002](adr/0002-local-dev-wrapper.md)).
   - The Enfusion MCP attaches automatically once the Net API (port 5775) is up.
3. Edit scripts in your editor (or with Claude); edit prefabs and worlds in Workbench. Reload scripts, test in Play mode.
4. Validate: `mod_validate` per addon, then `node tools/validate.mjs`.
5. Build (optional): `mod_build` with `outputPath` = `<repo>\build\<addon>`.
6. Multiplayer check: local dedicated server with `server/configs/dev.json` + a second client.
7. Run the Autotest suites in `NML_Tests`.
8. `git status` — no stray generated files, no missing `.meta`, no `Scripts/WorkbenchGame/EnfusionMCP/` in `addons/`.
9. Commit (Conventional Commits) and open a PR.

## Local dev wrapper (`.local/NML_Dev`)

Local-only and gitignored. The Enfusion MCP copies its handler scripts into the wrapper, never into `addons/`. Each developer creates `.local/NML_Dev/NML_Dev.gproj` once; use any random 16-hex `GUID` (it is never referenced):

```
GameProject {
 ID "NML_Dev"
 GUID "<random 16 hex>"
 TITLE "NML_Dev (local wrapper, never committed)"
 Dependencies {
  "58D0FB3206B6F859"
  "C175C744D88BE5AE"
  "8C44BDA9D3046928"
  "2F33881926E82E22"
  "99E85DF2DA22A8D2"
 }
 Configurations {
  GameProjectConfig PC {
  }
  GameProjectConfig HEADLESS {
  }
 }
}
```

Dependencies, in order: base game, `NML_Core`, `NML_Content`, `NML_Scenario_Dev`, `NML_Tests`. Add new NML addons here when they are created.

## Claude Code usage

- Research before writing: Enfusion MCP `api_search`, `wiki_search`, `game_read` of the vanilla class being extended.
- Features: `/plan` first; after changes, ECC `code-review` (and `security-review` for RPCs, admin and persistence code).
- Claude works on feature branches only; never pushes to `main`, never publishes to the Workshop, never deploys servers.
- `graphify update .` after code changes (docs/config map; weak on Enforce classes).

## Dependency changes

See [dependencies/README.md](../dependencies/README.md). Summary: candidate file → evaluation → **lead approval** → registry entry + `.gproj`/server-config change in one PR.
