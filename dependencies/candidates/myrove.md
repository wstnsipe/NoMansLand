---
name: Myrove
workshopId: 6A3C510B132310CA
status: proposed
category: foundational
capabilities: terrain
proposedBy: weston
proposedOn: 2026-10-03
channel: stable
licenseClass: APL
usageRestriction: 
isCollection: false
sizeMB: 191.05
builtForGameVersion: 1.8.0.13
lastUpdated: 2026-10-03
clientRequired: unknown
requires: 672166DE5926F11F, 66FC1E0F608A4AFA, 62EB4D903D542287, 6206C7238516657B, 61330C2DC7724DEA, 69770D09755CE1DC, 68D6FA70573ED4FE, 660EFAA574D2ED8C, 685B702A27B57341, 65A2EA40DC9E632A
requiredBy: 
referenceServers: 
approvedBy: 
approvedOn: 
---

# Myrove

Link: https://reforger.armaplatform.com/workshop/6A3C510B132310CA

World: `{4BF701326CC65F35}Myrove_Map.ent`. Author: Dingus-6ac71f088fa5d75e. Tag: TERRAINS.

## Purpose for NML

The intended NML terrain (lead decision, 2026-10-02). It would be a dependency of the future `NML_Scenario_Myrove` addon only; Core and Content stay terrain-independent ([ADR 0006](../../docs/adr/0006-myrove-integration.md)).

## Prerequisite chain

The Workshop page lists these 10 direct dependencies, without versions. The version and licence shown are each package's current values on its own Workshop page:

- Minus Building Pack (`672166DE5926F11F`), 1.0.68, APL-ND
- Malvian Bushwar Props Pack (`66FC1E0F608A4AFA`), 1.0.26, APL
- CIE Props (`62EB4D903D542287`), 1.0.111, APL-ND
- Crocell's Workshop of Horrors (`6206C7238516657B`), 1.0.40, APL-SA
- Assets for Tools by Heine (`61330C2DC7724DEA`), 2.0.4, APL-SA
- RUINS models (`69770D09755CE1DC`), 1.0.2, APL
- Ulups Assets (`68D6FA70573ED4FE`), 0.0.5, APL-ND
- Booses Bodies (`660EFAA574D2ED8C`), 0.0.2, APL-ND
- Snowzin Trenches Pack (`685B702A27B57341`), 1.0.12, APL
- Warfare-Colormod (`65A2EA40DC9E632A`), 1.0.13, APL

PR #6 research records no transitive dependencies beyond these 10.

## Verified facts and unknowns

| Item | Value | Source |
|---|---|---|
| Version | 1.0.5 | Workshop page, 2026-10-03 |
| Size | 191.05 MB for Myrove alone. With the 10 dependencies the closure is about 5.1 GB | Workshop pages |
| Built for game version | 1.8.0.13 | Workshop page |
| Last updated | 2026-10-03 (the day this was compiled) | Workshop page |
| Licence as declared | Arma Public License (APL) | Workshop page |
| Usage restriction | Not stated on the page | Workshop page |
| Client required | Not stated on the page. Unknown | Workshop page |
| Channel | Not stated on the page. Treated as stable, unverified | Workshop page |
| Version the spike used | PR #6 research recorded 1.0.4 (inspected local copy); dependency versions were not recorded | PR #6 research |
| Testing on the current version | None. The spike (Workbench, dedicated server, one client) ran on the earlier local copy | PR #6 research |

## Conflict surface

Not yet evaluated. PR #6 research recorded: no navmesh or AIWorld, no 2D topographic map data, horror and dead-body props, and a forced Warfare-Colormod colour grade. These are accepted characteristics ([ADR 0006](../../docs/adr/0006-myrove-integration.md)). The vanilla classes the package `modded`s or replaces have not been inspected.

## Overlap with other candidates

None known. It is the only terrain candidate.

## Native NML instead?

No. A terrain is external content.

## Multiplayer / performance risk

Closure of about 5.1 GB downloaded by every client. No FPS, server-load or endurance measurement exists. Versions change often: the page shows 1.0.5 while the earlier local copy was 1.0.4.

## Questions for Weston

1. Myrove declares APL; four of its dependencies declare APL-ND and two declare APL-SA. How should the mixed set be reviewed for NML's use as a dependency-only terrain? Recorded as a question; no conclusion is drawn.
2. Which version does TEST pin first: 1.0.5 (current) or the 1.0.4 the spike used? Batch 1 needs a run on the pinned version either way.
3. The page does not state whether clients must download it. Should that be confirmed during batch 1?

## Recommendation

Proposed for review. Nothing here is approved or registered.

## Status history

- 2026-10-03 proposed (compiled by Claude for Weston)
