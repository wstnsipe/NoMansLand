# ADR 0003: Third-party mods as a living candidate backlog with explicit approval

- **Status:** Accepted (2026-09-30)

## Context

Members suggest mods continuously. Many overlap, conflict, need deep prerequisite chains, carry restrictive licenses (GPL, APL-ND, server-restricted), or duplicate what NML should build natively. The list will keep growing; absence of a category does not mean it is settled.

## Decision

- One file per candidate in `dependencies/candidates/`; batch intake notes in `candidates/intake/`. A capability map (`capabilities.md`) drives overlap detection.
- Status lifecycle: `proposed → under-review → shortlisted → approved (lead only) → registered`, or `rejected`, `superseded`, `native`, `parked`.
- `dependencies/mods.json` holds only approved, registered mods. `tools/validate.mjs` rejects any `.gproj` dependency or server-config mod that is not in the registry, and any registry entry whose candidate is not approved.
- Policies: never vendor/modify third-party files; no collection items; no usage-restricted mod without a permission record; no Dev-channel mod on LIVE without explicit approval; TEST/LIVE pin versions.
- Claude never promotes a candidate to `approved` or `registered`.

## Consequences

New candidates never force restructuring: integration code lives in `NML_Compat_<Mod>` addons and pending capabilities get a service boundary in NML_Core.
