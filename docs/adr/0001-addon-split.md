# ADR 0001: Core + Content addon split, terrain-independent

- **Status:** Accepted (2026-09-30). Terminology updated 2026-10-03: the expected custom terrain is Myrove (Mangrove's map); no decision changed.

## Context

NML is a long-lived multiplayer project with several developers. Addon boundaries and resource GUIDs are expensive to change later (GUIDs are seeded per addon path). A custom terrain (Myrove, from Mangrove) is expected but not available; the gameplay must not depend on it.

## Decision

- `NML_Core` (scripts, configs, UI, game mode) and `NML_Content` (prefabs, assets) as separate addons released in lockstep. A code change does not force players to re-download art.
- Scenario addons (`NML_Scenario_Dev` now, `NML_Scenario_Myrove` later) are the only place that references a world. Core/Content never do.
- `NML_Tests` is never published. Third-party integration goes in `NML_Compat_<Mod>` addons.
- Creator tag `NML_` for every class, enum, file and addon.

## Consequences

- Terrain can be swapped by adding a scenario addon; Core/Content are untouched.
- Two Workshop items to publish per release (lockstep versioning keeps this simple).
