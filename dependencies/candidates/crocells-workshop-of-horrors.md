---
name: Crocell's Workshop of Horrors
workshopId: 6206C7238516657B
status: proposed
category: content
capabilities: terrain-dependency
proposedBy: weston
proposedOn: 2026-10-03
channel: stable
licenseClass: APL-SA
usageRestriction: 
isCollection: false
sizeMB: 1030
builtForGameVersion: 1.2.0.124
lastUpdated: 2024-09-22
clientRequired: unknown
requires: 
requiredBy: 6A3C510B132310CA
referenceServers: 
approvedBy: 
approvedOn: 
---

# Crocell's Workshop of Horrors

Link: https://reforger.armaplatform.com/workshop/6206C7238516657B

Author: Crocell_. Tags: EFFECTS, MISC, PROPS.

## Purpose for NML

A direct dependency of Myrove (`6A3C510B132310CA`). NML would register it only because Myrove needs it; NML does not use its content directly.

## Prerequisite chain

None listed on the Workshop page. PR #6 research recorded no transitive dependencies for the Myrove closure.

## Verified facts and unknowns

| Item | Value | Source |
|---|---|---|
| Version | 1.0.40 | Workshop page, 2026-10-03 |
| Size | 1030 MB | Workshop page |
| Built for game version | 1.2.0.124 | Workshop page |
| Last updated | 2024-09-22 | Workshop page |
| Licence as declared | Arma Public License Share Alike (APL-SA) | Workshop page |
| Usage restriction | Not stated on the page | Workshop page |
| Client required | Not stated on the page. Unknown | Workshop page |
| Channel | Not stated on the page. Treated as stable, unverified | Workshop page |
| Version the spike used | Not recorded | PR #6 research |

Notes from the page: Size shown as 1.03 GB. Built for game version 1.2.0.124 and last updated 2024-09-22, far older than the current game version 1.8.0.13.

## Conflict surface

Not yet evaluated. This looks like a content package; whether it `modded`s any vanilla class has not been inspected.

## Overlap with other candidates

None known. Required by Myrove (see [myrove.md](myrove.md)).

## Native NML instead?

No. It is external terrain content.

## Multiplayer / performance risk

Adds 1030 MB to the client download. No performance measurement exists.

## Questions for Weston

1. The page declares Arma Public License Share Alike (APL-SA). The licence name says share alike. Does NML's use as a dependency-only package need any review beyond recording this? Recorded as a question; no conclusion is drawn.
2. The package was built for an old game version (1.2.0.124, last updated 2024-09-22). The spike loaded it, but should it be a watch item in compatibility batch 1?

## Recommendation

Proposed for review. Nothing here is approved or registered.

## Status history

- 2026-10-03 proposed (compiled by Claude for Weston)
