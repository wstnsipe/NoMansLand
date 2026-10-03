---
name: CIE Props
workshopId: 62EB4D903D542287
status: proposed
category: content
capabilities: terrain-dependency
proposedBy: weston
proposedOn: 2026-10-03
channel: stable
licenseClass: APL-ND
usageRestriction: 
isCollection: false
sizeMB: 1230
builtForGameVersion: 1.8.0.13
lastUpdated: 2026-09-25
clientRequired: unknown
requires: 
requiredBy: 6A3C510B132310CA
referenceServers: 
approvedBy: 
approvedOn: 
---

# CIE Props

Link: https://reforger.armaplatform.com/workshop/62EB4D903D542287

Author: EU Conflict. Tags: PROPS.

## Purpose for NML

A direct dependency of Myrove (`6A3C510B132310CA`). NML would register it only because Myrove needs it; NML does not use its content directly.

## Prerequisite chain

None listed on the Workshop page. PR #6 research recorded no transitive dependencies for the Myrove closure.

## Verified facts and unknowns

| Item | Value | Source |
|---|---|---|
| Version | 1.0.111 | Workshop page, 2026-10-03 |
| Size | 1230 MB | Workshop page |
| Built for game version | 1.8.0.13 | Workshop page |
| Last updated | 2026-09-25 | Workshop page |
| Licence as declared | Arma Public License No Derivatives (APL-ND) | Workshop page |
| Usage restriction | Not stated on the page | Workshop page |
| Client required | Not stated on the page. Unknown | Workshop page |
| Channel | Not stated on the page. Treated as stable, unverified | Workshop page |
| Version the spike used | Not recorded | PR #6 research |

Notes from the page: The page describes it as "Ukrainian Props for mapmakers" (buildings, cars, fences and similar). Size shown as 1.23 GB.

## Conflict surface

Not yet evaluated. This looks like a content package; whether it `modded`s any vanilla class has not been inspected.

## Overlap with other candidates

None known. Required by Myrove (see [myrove.md](myrove.md)).

## Native NML instead?

No. It is external terrain content.

## Multiplayer / performance risk

Adds 1230 MB to the client download. No performance measurement exists.

## Questions for Weston

1. The page declares Arma Public License No Derivatives (APL-ND). The licence name says no derivatives. Does NML's use as a dependency-only package need any review beyond recording this? Recorded as a question; no conclusion is drawn.

## Recommendation

Proposed for review. Nothing here is approved or registered.

## Status history

- 2026-10-03 proposed (compiled by Claude for Weston)
