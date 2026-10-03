---
name: Assets for Tools by Heine
workshopId: 61330C2DC7724DEA
status: proposed
category: content
capabilities: terrain-dependency
proposedBy: weston
proposedOn: 2026-10-03
channel: stable
licenseClass: APL-SA
usageRestriction: 
isCollection: false
sizeMB: 1250
builtForGameVersion: 1.6.0.54
lastUpdated: 2025-11-07
clientRequired: unknown
requires: 
requiredBy: 6A3C510B132310CA
referenceServers: 
approvedBy: 
approvedOn: 
---

# Assets for Tools by Heine

Link: https://reforger.armaplatform.com/workshop/61330C2DC7724DEA

Author: Heine.CRV. Tags: MISC.

## Purpose for NML

A direct dependency of Myrove (`6A3C510B132310CA`). NML would register it only because Myrove needs it; NML does not use its content directly.

## Prerequisite chain

None listed on the Workshop page. PR #6 research recorded no transitive dependencies for the Myrove closure.

## Verified facts and unknowns

| Item | Value | Source |
|---|---|---|
| Version | 2.0.4 | Workshop page, 2026-10-03 |
| Size | 1250 MB | Workshop page |
| Built for game version | 1.6.0.54 | Workshop page |
| Last updated | 2025-11-07 | Workshop page |
| Licence as declared | Arma Public License Share Alike (APL-SA) | Workshop page |
| Usage restriction | Not stated on the page | Workshop page |
| Client required | Not stated on the page. Unknown | Workshop page |
| Channel | Not stated on the page. Treated as stable, unverified | Workshop page |
| Version the spike used | Not recorded | PR #6 research |

Notes from the page: The page says the assets are for the author's map in development and that the mod "will be changes frequently". Size shown as 1.25 GB.

## Conflict surface

Not yet evaluated. This looks like a content package; whether it `modded`s any vanilla class has not been inspected.

## Overlap with other candidates

None known. Required by Myrove (see [myrove.md](myrove.md)).

## Native NML instead?

No. It is external terrain content.

## Multiplayer / performance risk

Adds 1250 MB to the client download. No performance measurement exists.

## Questions for Weston

1. The page declares Arma Public License Share Alike (APL-SA). The licence name says share alike. Does NML's use as a dependency-only package need any review beyond recording this? Recorded as a question; no conclusion is drawn.
2. The author says the mod will change frequently. Should each version bump follow the ADR 0006 bump procedure?

## Recommendation

Proposed for review. Nothing here is approved or registered.

## Status history

- 2026-10-03 proposed (compiled by Claude for Weston)
