# ADR 0004: Medical behind a provider boundary

- **Status:** Accepted (2026-09-30); implementation choice deferred

## Context

Medical is undecided: ACE Medical, ATS, TCCC, or a native NML system. Vanilla medical lives in `SCR_CharacterDamageManagerComponent` (e.g. `ForceUnconsciousness`, `UpdateConsciousness`, `OnLifeStateChanged`), `SCR_CharacterBloodHitZone` and `SCR_BleedingDamageEffect`; the candidate mods replace or mod these same classes. Licenses differ: ACE Anvil is GPL-2.0-or-later (derivatives must be GPL), ATS is APL (derivatives allowed), TCCC is APL-ND (no derivatives).

## Decision

- `NML_Core/Scripts/Game/NML/Medical/`:
  - `NML_MedicalProviderBase` — abstract, config-instantiable provider (query `NML_MedicalState`, request treatment, force/clear unconsciousness, serialize for persistence, capability flags).
  - `NML_MedicalService` — server-authoritative facade used by all NML gameplay; raises `OnUnconscious`, `OnRevived`, `OnDeath`, `OnStateChanged`.
  - `NML_VanillaMedicalProvider` — default; the only NML file touching vanilla medical classes.
- `Configs/NML/Medical/NML_MedicalConfig.conf` selects the provider. Other providers live in `NML_Compat_ACEMedical`, `NML_Compat_ATS`, or a native `NML_Medical` addon.
- Medical items carry NML item-role tags; providers map roles to their items.
- An ACE-derived NML system is only allowed as a separate GPL-licensed addon/repository; otherwise native work is clean-room.

## Consequences

Choosing or changing the medical system is a config change plus an adapter addon, not a restructuring.
