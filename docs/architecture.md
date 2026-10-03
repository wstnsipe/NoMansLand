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

## Arsenal curation (implemented, vanilla only)

Implemented in Stage 5.3 and validated in Stage 5.4, as decided in [ADR 0005](adr/0005-arsenal-curation.md). The server decides what is obtainable from an arsenal; the client list is a courtesy filter.

- **Policy:** a faction-keyed whitelist with exact `ResourceName` matching (`NML_ArsenalPolicy`), referenced from `NML_CoreConfig.m_sArsenalPolicy`.
  - The shipped policy (`NML_Core/Configs/NML/Arsenal/NML_ArsenalPolicy.conf`) is **empty**, which means curation is disabled and vanilla behaviour is unchanged.
  - Once any faction entry exists, curation is active and an arsenal whose faction is missing or unlisted **fails closed**: an empty list on the client and a rejection on the server, with a once-per-key `[NML]` warning.
  - The key is the **arsenal's** assigned faction, not the player's.
- **Enforcement points** (the only two vanilla overrides, both in `Scripts/Game/NML/Modded/`):
  - client list: `SCR_ArsenalComponent.GetFilteredArsenalItems`;
  - server gate: `SCR_ResourcePlayerControllerInventoryComponent.RpcAsk_ArsenalRequestItem_`. It repeats the `[RplRpc(RplChannel.Reliable, RplRcver.Server)]` attribute, rejects unlisted items before vanilla handling and logs `[NML] Rejected arsenal request: player, arsenal faction, prefab`.
- **Test content:** vanilla US/USSR items only, in `NML_Tests` (never published): `NML_Test_Arsenal.ent` with a US, a USSR and a factionless arsenal. Procedure: [workflows](workflows.md#arsenal-curation-test-world-adr-0005).

### Proof (2026-10-03)

| Check | Result |
|---|---|
| Autotest `NML_TEST_ArsenalPolicySuite` | 9 of 9 pass: allowed items visible, banned filtered client-side, empty policy preserves vanilla, exact-match, once-per-key warning, missing or unlisted faction fails closed, server gate rejects banned, test world policy active, US/USSR separated |
| `NML_TEST_CoreSuite`, `NML_TEST_DevScenarioSuite` | pass |
| Dedicated diag server + two diag clients (one US, one USSR), each at the US, USSR and factionless arsenal | Each faction arsenal lists only its two allowed items. Forged banned requests (M72A3, RPG7) were rejected on the server with the correct player and arsenal faction and not delivered. Allowed requests (M855, 5.45 magazine) were delivered. The factionless arsenal listed nothing and rejected both forged requests, including the otherwise allowed magazine. |
| Cross-faction | A USSR player at the US arsenal and a US player at the USSR arsenal saw and could request that arsenal's items, and banned requests were rejected. Policy follows the arsenal, not the player (ADR 0005 decision 6). |
| DEV world (empty policy) | Server and client start, spawn and replicate with no script or RPC errors. The DEV world has no arsenal, so unchanged vanilla arsenal behaviour is covered by the empty-policy autotest case. |
| Validators | `mod_validate` (NML_Core, NML_Tests), `tools/validate.mjs`, validator and guard tests pass |

### Not yet implemented

- real Workshop arsenal content (the shipped policy is empty and the test policy uses vanilla items);
- rank locks and progression;
- role and quantity limits;
- enemy-arsenal restrictions (vanilla lets any player use any faction's arsenal);
- loadout restrictions: faction spawn loadouts and saved arsenal loadouts;
- modded acquisition paths (corpses, ground items, GM spawning, third-party arsenals or loadout editors);
- Myrove integration.

### Known limits

- Another mod that overrides the same RPC and does not call `super`, or that loads after NML, can bypass the gate. This goes on the Phase 6 compatibility checklist.
- Game updates can rename the RPC or add acquisition paths. Re-check after every update ([release checklist](release.md)).
- Rejections are logged per request. Flood handling is deferred.

## Other native capabilities (planned)

Rank locking (on vanilla `SCR_CharacterRankComponent`), persistent ranks, team balancing, spawn protection, loadout rules (e.g. grenade limits), HUD policy. See [dependencies/capabilities.md](../dependencies/capabilities.md).

## Environments

| Env | Mod source | Config |
|---|---|---|
| DEV | unpacked addons from your checkout via `-addonsDir` (`tools/wb-dev.ps1`, `tools/server-dev.ps1`, `tools/client-dev.ps1`) | `-server <world>` mode on the diag server ([workflows](workflows.md#local-multiplayer-dev-server--clients)); `server/configs/dev.json` once NML is on the Workshop |
| TEST | Workshop items at the release-candidate version, **pinned** | `server/configs/test.template.json` |
| LIVE | Workshop items at the promoted version, **pinned** | `server/configs/live.template.json` |

Server-config mod entries are `{modId, name, version?, required?}`; an omitted `version` means "latest", so TEST/LIVE must pin it (CI enforces this). Every listed mod is a required client download.

## Tooling notes

- **Enfusion MCP:** research first (`api_search`, `wiki_search`, `game_read`). Write tools always get an explicit `projectPath`; `mod_build` always gets `outputPath` under `build/`. The `.claude` guard hook enforces this.
- **Graphify:** parses Enforce Script as C — it maps files→symbols but misses class declarations and inheritance. Use `api_search` + Grep for class relationships.
- **Dedicated server:** Arma Reforger Server (Steam app 1874900) is required for local multiplayer tests. Local unpublished addons need its diag exe in `-server` world mode (see [workflows](workflows.md#local-multiplayer-dev-server--clients)).

## Open decisions (safe to defer)

License (interim: all rights reserved, source-visible; no reuse rights granted), Workshop publisher account, TEST/LIVE hosting, persistence backend (default: built-in Persistence System), medical system, Mangrove handoff, candidate mods.
