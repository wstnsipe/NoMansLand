# Capability map

A capability counts as settled **only** when marked `decided`. Anything not listed is `open`. Add new capabilities freely.

Statuses: `open` · `native-planned` · `provider-pending` · `decided`

| Capability | Status | Notes |
|---|---|---|
| Medical | provider-pending | Behind the NML_Core medical boundary ([ADR 0004](../docs/adr/0004-medical-boundary.md)). Candidates: ACE Medical, ATS, TCCC, native. |
| Arsenal curation | decided | Implemented natively (Stage 5.3, validated Stage 5.4, [ADR 0005](../docs/adr/0005-arsenal-curation.md)): faction-keyed config whitelist, server-authoritative, vanilla content only; the shipped policy is empty. Rank locking stays native-planned (`SCR_CharacterRankComponent`). See [architecture](../docs/architecture.md#arsenal-curation-implemented-vanilla-only) for what is not implemented yet. |
| Persistence | native-planned | Default backend: built-in Persistence System. |
| Ranks / progression | native-planned | Built on NML persistence (replaces FTAPersistentRanks idea). |
| Team balancing | native-planned | Server-authoritative. |
| Spawning / spawn protection | native-planned | Part of the NML spawn system. |
| Loadout rules (e.g. grenade limits) | native-planned | Rules in the native arsenal/loadout system. |
| HUD policy (nametags, markers, map position) | native-planned | Check vanilla game-mode/HUD settings first. |
| Suppression effects | open | Candidates compete (EE / GCS / LM). |
| Weapon sound overhaul | open | Candidates compete. |
| Voices / death audio | open | Possible coexistence; needs conflict check. |
| Explosion effects (tinnitus, concussion) | open | Candidates overlap. |
| Weapon / suppressor heat | open | ACE Overheating (Dev) vs 2-7 Suppressor Overheating. |
| Weapons & attachments content | open | |
| Uniforms / gear content | open | |
| Vehicles | open | |
| Grenades & mines content | open | |
| Radio / comms | open | |
| Drones | open | |
| Terrain / map | provider-pending | Mangrove map (external, not yet available); dev uses a base-game terrain. |
| AI | open | |
| Economy / trading | open | |
| Base building | open | |
| Survival / character stats | open | |
| Weather / time | open | |
| Admin / moderation tools | open | |
