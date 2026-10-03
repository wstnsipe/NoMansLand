---
name: Warfare-Colormod
workshopId: 65A2EA40DC9E632A
status: proposed
category: content
capabilities: terrain-dependency
proposedBy: weston
proposedOn: 2026-10-03
channel: stable
licenseClass: APL
usageRestriction: 
isCollection: false
sizeMB: 0.0007
builtForGameVersion: 1.4.0.45
lastUpdated: 2025-06-24
clientRequired: unknown
requires: 
requiredBy: 6A3C510B132310CA
referenceServers: 
approvedBy: 
approvedOn: 
---

# Warfare-Colormod

Link: https://reforger.armaplatform.com/workshop/65A2EA40DC9E632A

Author: G L 1 T C H. Tags: CC, COLOR, EFFECTS, MISC.

## Purpose for NML

A direct dependency of Myrove (`6A3C510B132310CA`). NML would register it only because Myrove needs it; NML does not use its content directly.

## Prerequisite chain

None listed on the Workshop page. PR #6 research recorded no transitive dependencies for the Myrove closure.

## Verified facts and unknowns

| Item | Value | Source |
|---|---|---|
| Version | 1.0.13 | Workshop page, 2026-10-03 |
| Size | 0.0007 MB | Workshop page |
| Built for game version | 1.4.0.45 | Workshop page |
| Last updated | 2025-06-24 | Workshop page |
| Licence as declared | Arma Public License (APL) | Workshop page |
| Usage restriction | Not stated on the page | Workshop page |
| Client required | Not stated on the page. Unknown | Workshop page |
| Channel | Not stated on the page. Treated as stable, unverified | Workshop page |
| Version the spike used | Not recorded | PR #6 research |

Notes from the page: The page shows the size as 0.68 KB (recorded as 0.0007 MB). PR #6 research recorded that it applies a global screen colour grade on every client.

## Conflict surface

Not yet evaluated. This looks like a content package; whether it `modded`s any vanilla class has not been inspected.

## Overlap with other candidates

None known. Required by Myrove (see [myrove.md](myrove.md)).

## Native NML instead?

No. It is external terrain content.

## Multiplayer / performance risk

Adds 0.0007 MB to the client download. No performance measurement exists.

## Questions for Weston

1. The page declares Arma Public License (APL). Is any further review of this licence needed for a dependency-only package? Recorded as a question; no conclusion is drawn.
2. The size shows as 0.68 KB, which looks like a display quirk. Should batch 1 record the real download size?

## Recommendation

Proposed for review. Nothing here is approved or registered.

## Status history

- 2026-10-03 proposed (compiled by Claude for Weston)
