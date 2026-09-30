# NML architecture

Decisions are recorded as ADRs in [adr/](adr/). This page is the overview.

## Addons

See [addons/README.md](../addons/README.md) and [ADR 0001](adr/0001-addon-split.md). Core and Content never reference a world; terrain-specific data (spawn zones, loot regions, safe zones) lives in a scenario addon and is discovered at runtime by NML_Core systems through placed entities/components, not hard-coded coordinates. Mangrove's map will be an external dependency of `NML_Scenario_Mangrove` only.

## Script organization

- `Scripts/Game/NML/<System>/` — one folder per system (`Persistence`, `Loot`, `Medical`, `Economy`, `Admin`, `Arsenal`, `Spawn`, `Common`, …). Each system exposes a server API and a client API.
- `Scripts/Game/NML/Modded/` — the **only** place for `modded class` overrides of vanilla, so every override can be found in one place.
- `Scripts/WorkbenchGame/NML/` — NML's own Workbench plugins/tools.
- Scripts outside `Scripts/{Core,GameLib,Game,WorkbenchGame,Workbench}` are ignored by the engine.

## Server vs client

- **Server (authoritative):** persistence, loot/spawn generation, economy, damage and medical outcomes, arsenal/rank gating, admin commands, validation of every client request (range, cooldown, ownership, state).
- **Client:** input, UI/HUD, prediction and effects, local audio/visuals. No gameplay decisions.
- Clients send requests, never outcomes. Gate with server/authority checks; confirm exact replication API names with the Enfusion MCP `api_search` before use.
- **All scripts ship to every client.** Secrets, admin lists, API keys and exploit-sensitive thresholds belong in the server profile/config, read at runtime.

## Service boundaries (pending decisions)

A capability gets an NML_Core service facade + config-selected provider only when a decision is actually pending there (a second competing candidate appears). Current boundary: **Medical** — see [ADR 0004](adr/0004-medical-boundary.md). Integration with third-party mods lives in `NML_Compat_<Mod>` addons so mods can be adopted, swapped or dropped without restructuring Core.

## Native capabilities (planned)

Arsenal curation + rank locking (on vanilla `SCR_ArsenalComponent` / `SCR_CharacterRankComponent`), persistent ranks, team balancing, spawn protection, loadout rules (e.g. grenade limits), HUD policy. See [dependencies/capabilities.md](../dependencies/capabilities.md).

## Environments

| Env | Mod source | Config |
|---|---|---|
| DEV | unpacked addons from your checkout via `tools/wb-dev.ps1` / `-addonsDir` | `server/configs/dev.json` |
| TEST | Workshop items at the release-candidate version, **pinned** | `server/configs/test.template.json` |
| LIVE | Workshop items at the promoted version, **pinned** | `server/configs/live.template.json` |

Server-config mod entries are `{modId, name, version?, required?}`; an omitted `version` means "latest", so TEST/LIVE must pin it (CI enforces this). Every listed mod is a required client download.

## Tooling notes

- **Enfusion MCP:** research first (`api_search`, `wiki_search`, `game_read`). Write tools always get an explicit `projectPath`; `mod_build` always gets `outputPath` under `build/`. The `.claude` guard hook enforces this.
- **Graphify:** parses Enforce Script as C — it maps files→symbols but misses class declarations and inheritance. Use `api_search` + Grep for class relationships.
- **Dedicated server:** Arma Reforger Server (Steam app 1874900) is required for local multiplayer tests (not installed yet on the lead's PC).

## Open decisions (safe to defer)

License (placeholder: proprietary), Workshop publisher account, TEST/LIVE hosting, persistence backend (default: built-in Persistence System), medical system, Mangrove handoff, candidate mods.
