# ADR 0006: Myrove terrain integration

- **Status:** Proposed (2026-10-03). Not accepted; Weston reviews before acceptance.

## Context

Myrove is NML's intended terrain (lead decision, 2026-10-02):

- Workshop ID and project GUID: `6A3C510B132310CA`.
- World: `{4BF701326CC65F35}Myrove_Map.ent`.
- 10 direct external dependencies, no further transitive ones; the closure is about 5.2 GB. The list and declared licences are in the [arsenal and resource handoff](../research/2026-10-02-arsenal-status.md#myrove-discovery).

A lead-run feasibility spike loaded Myrove and its closure in Workbench, on a world-mode dedicated server and with a joined client. Nothing is registered yet: `dependencies/mods.json` and the server configs list no Myrove entry.

[ADR 0001](0001-addon-split.md) already requires that only scenario addons reference a world and that Core and Content stay terrain-independent. [ADR 0003](0003-dependency-process.md) requires every third-party dependency to go through the candidate process and lead approval.

## Decision

1. **Scenario addon only.** A new `NML_Scenario_Myrove` addon is the only NML addon that depends on Myrove.
   - Its `.gproj` lists the base game, `NML_Core`, `NML_Content` and Myrove. The 10 Myrove dependencies come through Myrove's own project; they are still registered in `dependencies/mods.json` so the registry and the server mod lists record the full closure.
   - Its world sub-scenes `Myrove_Map.ent`. Terrain-specific data (spawn points, game mode, faction setup) lives in its layers.
   - It is an open-world faction sandbox: no objectives, capture points, rounds, score or win condition. The game mode is a lifecycle host only.
   - `NML_Core`, `NML_Content`, `NML_Scenario_Dev` and `NML_Tests` gain no Myrove dependency. Everon stays the DEV and test terrain.
2. **External and unmodified.** Myrove and its dependencies are Workshop dependencies only: never vendored, copied, edited or re-saved by NML. NML does not request changes to the map.
3. **Registration follows ADR 0003.** One candidate file per package (Myrove plus its 10 dependencies), lead approval, then registry entries, TEST pins and the scenario `.gproj` change.
4. **Version pinning.**
   - TEST and LIVE pin an exact `X.Y.Z` per package, equal to the registry version (already enforced by `tools/validate.mjs`).
   - Bump: candidate file updated, compatibility batch 1 (Myrove and its closure) rerun on a `deps/bump-<mod>-<version>` branch, lead approval, then registry and TEST updated. LIVE is promoted separately.
   - Rollback: revert the pin change. Verified in Stage 6.3 (probe with Warfare-Colormod, game 1.8.0.13): a dedicated server given `version` `1.0.12` while `1.0.13` was current downloaded exactly `1.0.12`, and a version that does not exist fails closed (the server refuses to start). The BI wiki says the Workshop keeps only the last 50 versions of a mod and deletes removed versions, so exact historical rollback is **not guaranteed**: it works only while the old version is still published.
5. **Local acquisition.** Local integration runs use a gitignored `.local/workshop/` folder filled through the official Workshop download of the dedicated server at the pinned versions. The game's own download folder (`Documents/My Games/ArmaReforger/addons/`) is not the integration source; read-only use for research or troubleshooting stays allowed.
6. **Accepted terrain characteristics.** These are accepted as they are unless one causes a direct integration defect, and are handled on NML's side:
   - no 2D topographic map data: map and HUD policy work around it;
   - no navmesh or AIWorld: the scenario has no AI; any future AI needs its own decision;
   - horror and dead-body props;
   - Warfare-Colormod applies a global colour grade on every client;
   - Myrove and its dependencies change version often: handled by pinning (decision 4).
7. **Licences are recorded, not concluded.** Declared licences (APL, APL-SA, APL-ND appear) and open questions go to Weston in the candidate files.

## Consequences

- Every TEST/LIVE client downloads the closure (about 5.2 GB), recorded by the validator's `[size]` line.
- Local test runs on Myrove need the `.local/workshop/` acquisition step; Everon runs do not change.
- Each Myrove or dependency update costs one batch-1 rerun before the pin moves.
- A later terrain swap is a new scenario addon; Core and Content are untouched (ADR 0001).

## Open points (not decided here)

- Final factions for the Myrove scenario: initially mirror the DEV setup; production factions wait on the faction and arsenal research.
- Approval of the 11 candidates, the TEST pins and the new addon GUID: separate lead gates in Stage 6.
- Whether `-addonsDir` accepts the repo addons and `.local/workshop/` together: verified during the tooling stage.
