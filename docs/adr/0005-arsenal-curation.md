# ADR 0005: Arsenal curation — enforcement points and faction-keyed policy

- **Status:** Accepted (2026-09-30); the lead resolved the six open decisions (see "Decisions resolved by the lead"). **Implemented in Stage 5.3 and validated in Stage 5.4 (see "Implementation status").**

## Context

NML is an open-world faction combat sandbox (no objectives, score, rounds or win condition). Opposing factions must have different obtainable equipment, and NML — not whichever content mods are loaded — decides what is obtainable. Phase 5 builds arsenal curation v1: a faction-keyed whitelist, vanilla content only, enforced by the server. A banned item must be rejected server-side even if a modified client requests it; UI-only filtering is not acceptable.

Research was read-only (Enfusion MCP `api_search` / `wiki_search` / `game_read`) against Arma Reforger 1.8. References below are to vanilla classes under `scripts/Game/`; no vanilla code is reproduced here.

### Vanilla flow

1. **Arsenal contents** — `SCR_ArsenalComponent.GetFilteredArsenalItems()` (`Components/Arsenal/`). If the arsenal has an overwrite config (`SCR_ArsenalItemListConfig`) it uses that; otherwise it asks `SCR_EntityCatalogManagerComponent.GetFilteredArsenalItems(types, modes, arsenalGameModeType, GetAssignedFaction())`, i.e. the **ITEM entity catalog of the arsenal's faction**, filtered by the arsenal's supported item types/modes. `GetAssignedFaction()` reads the arsenal's `SCR_FactionAffiliationComponent` (current or default faction).
2. **Where that list is used** — computed locally on every machine (the server only broadcasts type/mode flags via `RPC_OnArsenalUpdated`). Consumers:
   - the client inventory UI (`UI/Inventory/SCR_InventoryOpenedStorageArsenalUI`, `SCR_InventoryMenuUI`);
   - `SCR_ArsenalInventoryStorageManagerComponent.m_ItemsInArsenal` → `IsPrefabInArsenalStorage()`, used server-side by magazine resupply (`Inventory/SCR_InventoryStorageManagerComponent`), AI takes (`SCR_AITakeItemFromArsenal`) and the `IN_ARSENAL_ITEMS_ONLY` loadout-save check (`SCR_ArsenalManagerComponent`);
   - weapon racks (`SCR_ArsenalDisplayComponent.RefreshArsenal`), which spawn physical items.
3. **Client request** — every arsenal take/equip path in `SCR_InventoryMenuUI` calls `SCR_ResourcePlayerControllerInventoryComponent.RpcAsk_ArsenalRequestItem(arsenalResourceRplId, targetStorageRplId, prefab, resourceType)`. The client sends an arbitrary prefab `ResourceName`.
4. **Server authorization and execution** — `[RplRpc(Server)] RpcAsk_ArsenalRequestItem_` (`Components/Sandbox/SCR_ResourcePlayerControllerInventoryComponent.c`) checks: requester has a controlled character; target storage is not another character's; distances (30 m) and `ValidateStorageRequest`; an arsenal component exists; **item legitimacy = `GetEntryWithPrefabFromFactionCatalog(ITEM, prefab, arsenal.GetAssignedFaction())`**, falling back to the overwrite config only when no catalog entry exists; supply cost. Then `TrySpawnPrefabToStorage`.

### Gaps that matter for NML

- The server does **not** check the arsenal's filtered list or its type/mode flags — any prefab in the faction's ITEM catalog is accepted. Overwrite configs and UI filtering are therefore bypassable.
- The server does not compare the player's faction with the arsenal's faction.
- `s_OnArsenalItemRequested` fires only **after** the item is spawned, so it cannot gate a request.
- Catalog entries have an `m_bEnabled` flag, but disabled entries are dropped in `SCR_EntityCatalog.InitCatalog`; there is no public runtime removal, so catalogs cannot be curated at runtime without modding them.

### Factions

Vanilla keys: `"US"` (`Configs/Factions/US.conf`) and `"USSR"` (`Configs/Factions/USSR.conf`); each lists its ITEM catalog (`InventoryItems_EntityCatalog_<KEY>.conf`). Vanilla provides `FactionManager_USxUSSR.et`, `ArsenalBox_US.et` and `ArsenalBox_USSR.et` (faction affiliation US/USSR, resource component, resupply station, save type `FACTION_ITEMS_ONLY`).

## Decision

### Policy

- `NML_ArsenalPolicy` — a `[BaseContainerProps(configRoot: true)]` config shipped in NML_Core: `array<ref NML_ArsenalFactionPolicy>`, each with `m_sFactionKey` (a vanilla `FactionKey`) and `m_aAllowedPrefabs` (`array<ResourceName>`). Loaded once into a per-faction set.
- **Keyed by the arsenal's assigned faction** (vanilla semantics: the same faction that selects the ITEM catalog). The player's faction is not part of v1.
- **Whitelist, exact match:** for a listed faction, only listed prefabs are obtainable. Unknown or oddly formed strings fail closed.
- **Curation on/off and fail-closed semantics** *(decision 3)*:
  - **Policy has NO faction entries → arsenal curation is disabled** and vanilla behaviour is preserved everywhere. This is the Phase 5 production/default state.
  - **Policy has one or more faction entries → curation is active.** An arsenal whose assigned faction is **missing** (factionless) or has **no matching NML policy entry** **fails closed** for NML-curated requests: the client-filtered list for that arsenal is empty and the server rejects the request. Each rejection logs a clear `[NML]` warning (arsenal, faction key or "none", prefab); the "unlisted/factionless arsenal" warning is rate-limited to once per arsenal faction key to avoid log spam.
  - Rationale: a neutral or misconfigured arsenal must not become a whitelist bypass once curation is turned on, while an empty shipped policy stays non-disruptive.
- Referenced from `NML_CoreConfig` through one new `m_sArsenalPolicy` field. The policy shipped with NML_Core stays **empty in Phase 5**; restrictions used to prove the feature live only in NML_Tests. *(Decision 4.)*
- **No runtime server-only settings.** Clients need the same policy to filter their UI, it contains no secrets, and a server-only copy would only produce UI/server mismatches. The shipped config is sufficient.

### Client filtering point

`modded class SCR_ArsenalComponent` overrides `GetFilteredArsenalItems()`: call `super`, then remove items the policy disallows for `GetAssignedFaction()`. This runs on every machine, so it is **not UI-only**: it also restricts the server-side consumers above (arsenal storage membership, resupply, AI takes, weapon racks, `IN_ARSENAL_ITEMS_ONLY` saves). It covers vanilla arsenal prefabs (boxes, vehicles, GM-placed), not just NML-placed ones.

### Server enforcement point

`modded class SCR_ResourcePlayerControllerInventoryComponent` overrides `RpcAsk_ArsenalRequestItem_`: resolve the arsenal component from the resource component the same way vanilla does, check the policy for the arsenal's faction, and on a banned prefab log `[NML]` (player id, prefab, faction key) and return without calling `super`. Allowed requests go to `super` unchanged, so vanilla ownership, distance and supply checks still apply.

### Modded classes

**Required: exactly two**, both in `Scripts/Game/NML/Modded/`, adding only `NML_`-prefixed members:

- configuration alone cannot give server authority (the RPC trusts the whole faction catalog);
- a subclass alone misses vanilla arsenals and cannot reach the RPC.

### Minimal validation

Only the policy membership check at the two points above, plus an `[NML]` log line on rejection. Vanilla's ownership, distance and supply checks are not duplicated. No general request-validation framework.

### Test strategy

- The **DEV scenario stays FFA and unchanged.**
- A **dedicated NML_Tests world**, `Worlds/NML/Tests/NML_Test_Arsenal.ent` (sub-scene of Everon, like DEV), containing:
  - `GameMode_Plain` with `NML_CoreComponent` pointed at a test core config and a test policy (both in NML_Tests);
  - `FactionManager_USxUSSR`;
  - one `ArsenalBox_US` and one `ArsenalBox_USSR` (save type `IN_ARSENAL_ITEMS_ONLY`);
  - vanilla US/USSR spawn and loadout setup, so players pick a side via the vanilla deploy menu.
  - No objectives, capture points, score or rounds.
- **Autotest** `NML_TEST_ArsenalPolicySuite` (in the test world): the policy loads; allowed and banned prefabs are classified correctly per faction key (`US`, `USSR`); each arsenal's filtered list excludes its banned prefabs; an empty policy leaves vanilla behaviour unchanged; with entries present, an unlisted or factionless arsenal is rejected (fail closed).
- **Forged request:** a test-only diag-menu helper in NML_Tests sends `RpcAsk_ArsenalRequestItem` for a banned prefab from a client. `tools/server-dev.ps1` and `tools/client-dev.ps1` get an opt-in `-IncludeTests` switch so the server and clients load NML_Tests. NML_Tests remains never published.
- **Stage 5.4 two-client check:** a US player and a USSR player each visit both arsenals and see the correct list; a forged banned request is rejected and logged; an allowed item still works. The check uses the US and USSR arsenals because the policy is keyed by the arsenal's faction, not the player's. *(Decision 6.)*

## Rejected alternatives

| Alternative | Why rejected |
|---|---|
| UI or overwrite-config filtering only | The server accepts any faction-catalog item; bypassable. |
| Curated NML faction ITEM catalogs / custom factions | Config-only and vanilla-enforced, but duplicates large vanilla configs; ITEM catalogs also drive supply cost, identity items, loadout saves and the GM editor; requires faction-system work that is out of scope. |
| Subclass `NML_ArsenalComponent` only | Misses vanilla arsenals; cannot gate the RPC. |
| Post-spawn removal via `s_OnArsenalItemRequested` | Too late (item spawned, supplies spent); racy. |
| Replacing `SCR_EntityCatalogManagerComponent` | Broad side effects on every catalog consumer; requires reconfiguring game-mode catalogs. |
| Converting DEV to US/USSR or to an objective mode | Distorts the DEV scenario; not needed to test the policy. |
| Runtime server-only JSON settings | Not needed (see Policy). |

## Risks and limitations

1. **RPC override:** overriding an `[RplRpc]` method in a `modded` class must be proven first in Stage 5.3 (compile, then the forged-request test). Fallback: a `modded` `SCR_EntityCatalogManagerComponent.GetEntryWithPrefabFromFactionCatalog` limited to `ITEM` lookups — broader side effects, so only if the override does not work.
2. **Game updates** can rename the RPC or add new acquisition paths. Re-check this ADR's flow on every game update (to be added to the release checklist in 5.3).
3. **Out of v1 scope:**
   - gear from corpses, the ground or GM spawning;
   - faction spawn loadouts;
   - saved arsenal loadouts on `FACTION_ITEMS_ONLY` arsenals (they can include banned items picked up elsewhere);
   - players using enemy-faction arsenals.
   Loadout rules and faction-access rules are Phase 6 candidates.
4. Whitelist entries must match the catalog `ResourceName` exactly (`{GUID}path`).

## Stage 5.3 implementation outline (implemented)

1. `NML_Core/Scripts/Game/NML/Arsenal/NML_ArsenalPolicy.c` — policy + faction entry + membership check, including the "no entries = disabled" and fail-closed-for-unlisted/factionless rules.
2. `NML_Core/Scripts/Game/NML/Modded/SCR_ArsenalComponent.c` and `.../Modded/SCR_ResourcePlayerControllerInventoryComponent.c`. First step: prove the RPC override works.
3. `NML_CoreConfig.m_sArsenalPolicy` + an empty shipped `NML_Core/Configs/NML/Arsenal/NML_ArsenalPolicy.conf`.
4. NML_Tests: the arsenal test world (placed via Workbench), test core config + test policy, `NML_TEST_ArsenalPolicySuite`, diag forge helper.
5. `-IncludeTests` switch in `tools/server-dev.ps1` and `tools/client-dev.ps1`.

## Implementation status (2026-10-03)

Stage 5.3 implemented this ADR as written (PR #7, merge commit `568f363`); the `[RplRpc]` override worked, so the fallback was not needed. Stage 5.4 repeated the live check with a dedicated diag server and two diag clients, one US and one USSR. Results and the list of what is still not implemented are in [architecture.md](../architecture.md#arsenal-curation-implemented-vanilla-only). No decision in this ADR changed.

## Decisions resolved by the lead (2026-09-30)

1. **Approved:** the two modded vanilla classes are the minimum required path for client-visible filtering and real server-side enforcement. Stage 5.3 must first prove that overriding the `[RplRpc]` method works before relying on it; the documented fallback stays if the proof fails.
2. **Approved:** policy is keyed by the **arsenal's assigned faction key** (vanilla semantics). No player-versus-arsenal faction access rules in Phase 5; that remains deferred.
3. **Changed:** unlisted or factionless arsenals are **not** permanently unrestricted.
   - Policy with **no** faction entries = curation disabled, vanilla behaviour preserved (the Phase 5 default).
   - Policy with **one or more** faction entries = curation active; an arsenal whose faction is missing or has no NML policy entry **fails closed** (empty client list, server rejection) with a clear `[NML]` warning.
4. **Approved:** the shipped NML_Core policy stays empty in Phase 5; restrictions used to prove the feature live only in NML_Tests.
5. **Approved:** dedicated NML_Tests arsenal world, vanilla US/USSR factions, US and USSR arsenal boxes, opt-in `-IncludeTests` in the dev server/client scripts, and a test-only forged-request helper. No objectives, capture points, score, rounds or other match mechanics.
6. **Approved:** Stage 5.4 verifies US and USSR players using the US and USSR arsenals (policy follows the arsenal's faction, not the player's).

## Consequences

With an empty policy (the Phase 5 default) nothing changes for players; once any faction entry exists, every arsenal is curated and unlisted or factionless arsenals fail closed. Arsenal curation is server-authoritative at the single request entry point and consistent everywhere the vanilla arsenal list is consumed, with two small, documented vanilla overrides. Content mods can be added later without changing the mechanism; only the policy config grows. Loadout and faction-access rules remain separate, later decisions.
