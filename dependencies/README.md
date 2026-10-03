# Dependencies

Third-party mods (and the Myrove terrain with its dependencies) are **external dependencies only**: never vendored, copied or modified in this repository. See [ADR 0003](../docs/adr/0003-dependency-process.md).

| File | Purpose |
|---|---|
| `mods.json` | Registry of **approved, registered** dependencies only. Source of truth for `.gproj` dependencies and server-config mod lists. |
| `capabilities.md` | Functional areas and their status; drives overlap detection. |
| `candidates/` | Living backlog — one file per candidate, plus `intake/` batch notes and a generated `INDEX.md`. |

## Lifecycle

`proposed → under-review → shortlisted → approved (lead only) → registered`
End states: `rejected`, `superseded`, `native` (NML builds it), `parked`.

## Adding or evaluating a candidate

1. Open a "Mod candidate" issue (or tell the lead). Copy `candidates/_TEMPLATE.md` to `candidates/<slug>.md`.
2. Evaluate: capabilities touched, full prerequisite chain, client-required vs server-only, size, license class, usage restriction, conflict surface (vanilla classes it mods/replaces), overlap with other candidates, "native NML instead?".
3. Run `node tools/candidates-index.mjs` and open a PR. The PR must **not** touch `mods.json` or any `.gproj`.

## Registering an approved candidate

Only after the lead sets `status: approved` and `approvedBy`:
1. Add an entry to `mods.json` (fields below) pointing at the candidate file.
2. Add the GUID to the `.gproj` of the addon that truly needs it at load/compile time (optional/server-side mods go only in server configs).
3. Pin the version in `server/configs/test.template.json` (and LIVE when promoted).
4. `node tools/validate.mjs` must pass.

## Registry entry fields

`name`, `modId` (16 hex GUID), `version` (X.Y.Z), `purpose`, `required`, `scope` (core|content|scenario|server-only), `channel` (stable|dev), `licenseClass` (GPL|APL|APL-SA|APL-ND|custom), `usageRestriction` + `permissionRecord` if restricted, `sizeMB`, `builtForGameVersion`, `lastUpdated`, `requires` / `requiredBy`, `loadOrder`, `compatibilityNotes`, `testedWithNML`, `referenceServers`, `candidate`, `approvedBy`, `approvedOn`, optional `liveDevChannelApproved`.

## Policies (enforced by `tools/validate.mjs`)

- No collection items; depend on individual modules.
- No usage-restricted mod without a `permissionRecord`.
- No Dev-channel mod on LIVE unless `liveDevChannelApproved`.
- TEST/LIVE configs pin every mod version (omitted = latest).
- `requires` / `requiredBy` are arrays of 16-hex GUIDs. Every registered mod's `requires` must be registered, and both sides of an edge must agree (`A.requires` lists `B` exactly when `B.requiredBy` lists `A`); no cycles.
- A pinned TEST/LIVE config must list the complete registered closure of every mod it lists, and a scenario `.gproj`'s registered dependencies must have a complete registered closure.
- GPL mods are fine as dependencies; any derivative work must be GPL (separate addon/repo). APL-ND: dependency only.
