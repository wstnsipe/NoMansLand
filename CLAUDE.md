## graphify

This project has a knowledge graph at graphify-out/ with god nodes, community structure, and cross-file relationships.

Rules:
- For codebase questions, first run `graphify query "<question>"` when graphify-out/graph.json exists. Use `graphify path "<A>" "<B>"` for relationships and `graphify explain "<concept>"` for focused concepts. These return a scoped subgraph, usually much smaller than GRAPH_REPORT.md or raw grep output.
- If graphify-out/wiki/index.md exists, use it for broad navigation instead of raw source browsing.
- Read graphify-out/GRAPH_REPORT.md only for broad architecture review or when query/path/explain do not surface enough context.
- After modifying code, run `graphify update .` to keep the graph current (AST-only, no API cost).

## NML project

No Man's Land (NML): hardcore multiplayer Arma Reforger server/mod. Overview: `docs/architecture.md`; decisions: `docs/adr/`; daily loop: `docs/workflows.md`.

### Addons (`addons/`)
- `NML_Core` (scripts, configs, UI, game mode — no terrain references), `NML_Content` (prefabs, assets), `NML_Scenario_Dev` (dev scenario on a base-game terrain), `NML_Tests` (never published), `NML_Compat_<Mod>` (third-party integration), later `NML_Scenario_Myrove`.
- Local dev wrapper `.local/NML_Dev/` is gitignored. Start Workbench with `tools/wb-dev.ps1` (passes `-addonsDir`); the Enfusion MCP attaches after.

### Conventions
- Creator tag **`NML_`** on every class, enum (`NML_E…`), script file and addon. Allman braces, tabs, members `m_` + type letter.
- Scripts: `Scripts/Game/NML/<System>/`. `modded class` only in `Scripts/Game/NML/Modded/`, and every added member is `NML_`-prefixed.
- Content namespaced under `NML/` (e.g. `Prefabs/NML/…`).
- Server is authoritative; clients send requests, never outcomes. All scripts ship to clients — no secrets in the mod.
- Medical code goes through `NML_MedicalService` / provider (ADR 0004); never call vanilla medical classes from gameplay code.

### Research before writing (required)
- Use Enfusion MCP `api_search`, `wiki_search` and `game_read` of the vanilla class before writing or overriding anything. Never guess API names.
- Graphify parses Enforce Script as C: it misses class declarations and inheritance. Use `api_search` + Grep for class relationships.

### Enfusion MCP rules (enforced by `.claude/hooks/nml-guard.mjs`)
- `mod_create`: `projectPath` = `<repo>/addons`, name starts with `NML_`.
- `script_create` / `prefab_create` / `config_create` / `layout_create` / `project_write`: explicit `projectPath` of an addon under `addons/`.
- `mod_build`: `outputPath` under `<repo>/build/` (its default writes into Program Files).
- `wb_launch`: only with `.local/NML_Dev/NML_Dev.gproj` (prefer `tools/wb-dev.ps1`). `wb_cleanup`: only NML addons or the wrapper.

### .meta and GUIDs
- `.meta` files are committed and travel with their resource. Never delete or regenerate them; move/rename resources in Workbench. Never hand-edit GUIDs.

### Dependencies
- Third-party mods are external only — never vendor, copy or modify them. Process: `dependencies/README.md`.
- New suggestions go to `dependencies/candidates/` as `proposed`. The backlog is open-ended.
- Only the lead may set a candidate to `approved`/`registered`; Claude never does. `mods.json` and `.gproj` dependencies change only for approved candidates.

### Workflow
- Feature branches only (`feat/…`, `fix/…`, `chore/…`, `content/…`, `deps/…`); Conventional Commits; never push to `main`.
- Before a PR: `node tools/validate.mjs`, `mod_validate` per touched addon, ECC `code-review` (plus `security-review` for RPC, admin and persistence code).

### NML safeguards — never touch
Base game / Arma Reforger Tools / any `steamapps` folder; `Documents/My Games/ArmaReforgerWorkbench` (incl. the EnfusionMCP support addon) and `Documents/My Games/ArmaReforger`; the npm cache; `~/.claude.json`; `~/.claude/plugins`; production/server hosts; third-party mod folders; the Myrove map.

### Ask the user first
GUID changes; `.gproj` dependency changes; persistence schema changes; candidate promotion; anything under `server/`; publishing to the Workshop; deleting files; creating or pushing the GitHub repo (`wstnsipe/NoMansLand`).
