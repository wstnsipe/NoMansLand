# NML arsenal, resource and testing handoff — 2026-10-02

This publishes existing local research and member-reported tests. It does not perform new gameplay testing or implementation. Research and member preferences do not equal Weston approval.

Read the [research summary](2026-10-01-mod-candidate-review.md), [87-package inventory](2026-10-02-mod-inventory.md), [operating rules](../AI_OPERATING_RULES.md) and [workboard](../AI_WORKBOARD.md). Stage 5.3 remains blocked. Third-party Workshop content remains external and unmodified.

## Current arsenal status

**No finished item-level whitelist, production arsenal prefab, approved role loadouts or complete item-resource inventory exists from this work.** The Workshop list identifies possible content providers, not every item that should be obtainable.

The initial pasted handoff proposed a small NML-owned box using `SCR_ArsenalItemListConfig`, followed by Bacon integration experiments. That is research history. It does not replace [accepted ADR 0005](../adr/0005-arsenal-curation.md): the first implementation is vanilla-only, keyed by the arsenal's assigned faction, with client-visible filtering and server enforcement. The shipped policy stays empty; restrictive tests belong in NML_Tests. DEV remains FFA and unchanged. Player-versus-arsenal faction access and other acquisition routes are outside that first implementation scope.

### Confirmed member preferences

These are confirmed conversation choices, not production decisions.

| Area | Preference / current state |
|---|---|
| Factions | Two opposing sides with Ukrainian/Russian-inspired equipment; fictional display names and actual keys/configuration unresolved. |
| Roles | No finalized role list or role-specific equipment. |
| Weapons | MCR and FN SCAR explicitly requested; GRS Weapons 2, HK-G95KA1, MSBS Grot, FORTEX weapons and shotguns remain content candidates. No exact variants approved. |
| Attachments/optics | Bacon/GRS suppressors, RIS equipment, SW-Reap-IR and VooDoo clip-on retained for evaluation; exact compatible weapon/optic combinations unresolved. |
| Ammunition | No item-level whitelist. FORTEX ammunition and SCAR-H custom magazines are available content candidates, not approved entries. |
| Launchers/explosives | Extra Grenades, Impact Grenades and Mines chosen for testing; RGM-40/GL-06, thermobaric grenade and colored smoke retained. Limits/access unresolved. |
| Clothing | Newer Minnesinger Clothes/Core/Gear/Headwear plus Zeliks; FORTEX gear and both Ukraine packs retained for comparison. |
| Armor | No approved helmet/vest/rig/plate list, protection values or item restrictions. |
| Backpacks | No approved backpack list or capacity policy. |
| Medical | Matching ACE Dev Core/Medical Core/Hitzones/Circulation/Breathing plus SplintACE preferred for evaluation; exact items, quantities and settings unresolved. |
| Utility | ATAK equipment, map aids, rangefinder and other utilities retained as candidates; visibility/availability rules unresolved. |
| Vehicles | Brown VT4 BTDF/BTDF Equipped proposed for Ukrainian-inspired side; green CE/CE Equipped for Russian-inspired side. Actual team assignment unverified. |
| Camera/HUD | CERTII-SimulatedMovement, Best Body Cam, CERTII-Minimal-HUD and CERTII-Grenade-Limiter retained. SlavicWar Bodycam and Immersive Head Movement excluded. |
| Progression | FTA retained as an evaluation candidate; not approved to replace NML's planned systems. |

### Recommendations, not approved decisions

- Establish a reproducible, small test configuration before combining the entire list.
- Discover exact `{GUID}path` resource references and curate intended items rather than importing every item supplied by loaded mods.
- Verify magazines, attachment slots, armor, inventory capacity, medical interactions and acquisition routes separately.
- Determine how Bacon enumerates items and whether NML server restrictions cover its acquisition paths before relying on it as the production interface.
- Keep original Workshop projects and assets external. Any NML compatibility work requires a separately approved scope and applicable license permission.

### Experimental ideas

- Easier early ranks and progressively harder higher ranks.
- Separately tuned kill/transport/medical XP; low medical XP preferred.
- Manual or timer-based rank-reset policy; no values or reset configuration applied.
- Fictional faction names usable across multiple maps; none chosen.
- Brown/green VT4 allocation; no completed affiliation or faction catalog work.
- Tiny scratch overwrite-config arsenal/Bacon visibility comparison; not an alternative approved architecture.
- ChangeYourFace, Bacon Arsenal Only and IRBA remain optional/deferred, not selected dependencies.

## VT4 findings

- Workshop ID/project GUID: `663B2784961621FB`; installed version `1.0.11`.
- The member placed and reported driving the BTDF variant in FritV3's `test world.ent`, a sub-scene of vanilla MpTest.
- A local child and two world instances exist. The child resource is named `NML_VT4_BTDF.et.et` (double extension), GUID `7328AFDA667CFC99`.
- Child parent: `{1AD136D5D29A4C99}Prefabs/Vehicles/Wheeled/VT4/VT4_BTDF.et`.
- Child contains the parent reference and coordinates, with no saved faction-affiliation override. Screenshot affiliation was `CIV`.
- Brown BTDF and green CE were proposed for opposite sides. Both regular and Equipped variants were observed, but Equipped contents have not been inspected; do not assume that Equipped means armed.
- FritV3 GUID is `6A84509490402956`. Its saved `.gproj` lists **only vanilla** (`58D0FB3206B6F859`). Historical Workbench log confirms VT4 was supplied as an additional addon; it is not a persisted project dependency.
- No NML repository resource or production vehicle catalog was created. This scratch work does not establish server deployment or team-restricted spawning.

Historical logs contain wrong GUID/name references for `{536BF67B2052B869}material/metal.gamemat`, failed material/resource opens, brake-light `HierarchyComponent` warnings, `SCR_BaseEditorComponent must be attached to SCR_EditorBaseEntity!`, and duplicate editor-notification keys. Screenshots also showed cargo-seat action/tablet warnings. Root causes, severity and fixes were not established. A successful drive test does not establish error-free behavior.

## Myrove discovery

- Workshop link: [Myrove](https://reforger.armaplatform.com/workshop/6A3C510B132310CA).
- Workshop ID and locally inspected project GUID: `6A3C510B132310CA`.
- Installed metadata version: `1.0.4`.
- Project file found locally; no complete payload/manifest audit or gameplay test claimed for this package.
- Outside the current 87-package candidate/download inventory. Workboard evaluation remains **Planned**, owner **TBD**.
- **Full dependency-name, transitive-dependency, installation-completeness and compatibility evaluation is pending.** Do not treat discovery as approval or add these dependencies automatically.

Direct external dependency GUIDs from the installed project, excluding vanilla:

```text
672166DE5926F11F
66FC1E0F608A4AFA
62EB4D903D542287
6206C7238516657B
61330C2DC7724DEA
69770D09755CE1DC
68D6FA70573ED4FE
660EFAA574D2ED8C
685B702A27B57341
65A2EA40DC9E632A
```

## Other locally discovered projects — not selected additions

These project files/metadata were found locally. Contents, complete dependency evaluation, compatibility and public-server permissions have not been established. They are outside the 87-package set.

| Project | Workshop ID / inspected GUID | Local version | Direct external dependencies / evaluation limits |
|---|---|---|---|
| SlavicWar Drones | `6A0A6A381CEECD19` | 1.0.1 | Original Realistic Combat Drones `65AD60E204191D37`; additional behavior uninspected. |
| SlavicWar VT4 retexture | `6A6342CEC897C4A3` | 1.0.0 | VT4 FRM `663B2784961621FB`; not required for observed base BTDF/CE variants. |
| SlavicWar Patches | `6A427D8609F4F25A` | 1.0.30 | `68472D8D8A663E90`, `66C2ED22F25B05C9`, `64722DADC53CB75E`, `645F08FA9E7CDEDE`, `687CD82F6E41D627`, `6A02B35D22BF34F1`, `606B100247F5C709`. |
| SlavicWar Arsenal V2 | `6A26C05203680B3B` | 1.0.44 | Direct GUID list below; exact catalog behavior and complete closure uninspected. |

SlavicWar Arsenal V2 direct external GUIDs, excluding vanilla:

```text
6738DB574AAA1CB4 65A936725A71733C 645F08FA9E7CDEDE
687CD82F6E41D627 672001F640D12CD3 66C2ED22F25B05C9
6A02B35D22BF34F1 68472D8D8A663E90 68F92DCEEC950E1C
690AC3895BAAFAE1 69293A8ED08A262D 64722DADC53CB75E
6A65091824BEC7DF 6A6507D034ECFC69 6A6509AC32B5A49A
67191670195CBA05 65117E01FC88FE08 6528F296E88F783F
5D0551624969C92E 5ABD0CB57F7E9EB1 62A668F513428630
68F006D910E7546F 631A9C6C92283C44 68FC03DFB1E2F1C7
64656BF043969017 65CF7AE8574E06D2 67126A42BAE3B5EB
69148A1D52696666 66D743AE865D843E
```

## Exact known resources and identifiers

| Resource / identifier | Evidence |
|---|---|
| `scripts/Game/Components/Arsenal/SCR_ArsenalComponent.c` | Previously read from installed vanilla `.pak` data through Enfusion MCP. |
| `Prefabs/Props/Military/Arsenal/ArsenalBoxes/ArsenalBox_Base.et` | Previously read from installed vanilla data. |
| `{1AD136D5D29A4C99}Prefabs/Vehicles/Wheeled/VT4/VT4_BTDF.et` | Exact reference in scratch child and world layer. |
| `{7328AFDA667CFC99}NML_VT4_BTDF.et.et` | Scratch child resource and `.meta`. |
| `{96A8AF57260A7392}worlds/MP/MpTest/MpTest.ent` | Parent of FritV3 test world. |
| `Prefabs/Vehicles/Wheeled/VT4/VT4_BTDF_Equipped.et` | Observed in Workbench screenshots/catalog. |
| `Prefabs/Vehicles/Wheeled/VT4/VT4_CE.et` | Observed in Workbench screenshots/catalog. |
| `Prefabs/Vehicles/Wheeled/VT4/VT4_CE_Equipped.et` | Observed in Workbench screenshots/catalog. |
| `Prefabs/Vehicles/Wheeled/VT4/VT4_base.et` | Observed parent resource. |
| `Configs/EntityCatalog/US/VT4_list.conf` | Observed under VT4FRM. |
| `Configs/EntityCatalog/US/Vehicles_EntityCatalog_US.conf` | Observed under VT4FRM; not proof of NML availability. |
| `$BaconLoadoutEditor:Configs/Editor/AttributeLists/Edit.conf` | Supplied earlier handoff records read-only save failure. Exact path preserved as reported; not proved to be arsenal integration. |
| `Configs/BaconLoadoutEditor/Editor/EntityCatalog/FIA`, `/US`, `/USSR` | Earlier observed catalog areas; individual catalog filenames/integration behavior not resolved. |

Relevant classes/settings from vanilla research, ADRs or screenshots:

```text
SCR_ArsenalComponent
SCR_ArsenalItemListConfig
SCR_ArsenalManagerComponent
SCR_ArsenalInventoryStorageManagerComponent
SCR_ArsenalDisplayComponent
SCR_EntityCatalogManagerComponent
SCR_EntityCatalogMultiList
SCR_EntityCatalogEntry
SCR_ResourcePlayerControllerInventoryComponent
SCR_FactionAffiliationComponent
SCR_VehicleFactionAffiliationComponent
SCR_FactionManager
SCR_DelegateFactionManagerComponent
Overwrite Arsenal Config
FACTION_ITEMS_ONLY
IN_ARSENAL_ITEMS_ONLY
```

Vanilla keys are `US`, `USSR`, `FIA`, `CIV`. `UA`/`RU` were descriptive intentions, not verified configured keys. No fictional names were finalized. ADR 0005's initial tests use `US` and `USSR`.

Proposed scratch `Prefabs/Arsenal/NML_Arsenal.et` and `Configs/Arsenal/NML_Arsenal.conf` were not created by this work. No complete exact resource list exists for modded weapons, ammunition, armor, backpacks or medical items.

## Testing and tooling evidence

| Activity | Evidence / result |
|---|---|
| VT4 BTDF driving | Member report in FritV3/MpTest; placement/child files confirmed. Codex did not independently drive it. |
| CERTII + Best Body Cam | Member reports they work together; scenario/settings not recorded; no independent Codex reproduction. |
| Bacon original config save | Earlier handoff: failed because original dependency resource was read-only. Not a gameplay failure. |
| Complete 87-package setup | Not gameplay-tested; no dedicated-server/two-client/performance result. |
| New NML arsenal/Bacon whitelist | Not built or tested. |
| SlavicWar Bodycam combination | Not tested; comparison cancelled. |
| Workshop downloads | Local identity/payload/manifest-size checks for 86 current packages; MCR download completed through Codex. Not a runtime test. |
| Enfusion research tools | Previously verified vanilla API search, source reading, wiki search and asset discovery. Bundled API/wiki index is not proof every signature matches installed game. |
| Workbench bridge | Previously verified health-check ping in separate vanilla-only project at `127.0.0.1:5775`; no NML handler injection or gameplay claim. |

Recorded publication checks for the earlier PR: repository validator 0 errors/0 warnings; validator tests 27/27; guard tests 47/47; candidate index current; CI run `36956827220` successful. Those results concern repository rules, not mod compatibility. Current publication validation/CI is reported on PR #6 rather than inferred from the earlier run.

Arma Reforger and Tools were discovered in the standard Steam `steamapps/common/Arma Reforger` and `steamapps/common/Arma Reforger Tools` folders. Dedicated-server installation/path remains unverified. No FPS, server-load or endurance benchmark exists.

Author-reported limitations remain untested here: Maxxpro M134 turret unresolved; 2-7 server smoke needs server testing; VooDoo needs compatible RIS weapon/optic; several weapon packages target older game versions; FORTEX and 2-7 heating overlap. PC-focused blood-effect preference is not a performance result.

## Weston decisions and unresolved work

1. Approve final V1 dependency set and versions; member selections have not changed repository lifecycle status.
2. Choose RTS/RHS equipment providers and whether combined content is justified.
3. Finalize faction display names and actual faction configuration; assess RHS Fix's faction replacement.
4. Define roles and exact per-faction weapon, magazine, ammunition and attachment whitelist.
5. Approve armor/storage/backpack policies and equipment quantities.
6. Decide thermal, drone, mine, grenade and launcher access/balance restrictions.
7. Choose medical configuration, supplies and approved provider integration under ADR 0004; no adapter implemented.
8. Resolve FORTEX native heating versus 2-7 behavior.
9. Test ACE/CERTII/GC/ragdoll/Keep Gun injury, unconsciousness, camera and weapon-retention interactions.
10. Record a reproducible CERTII + Best Body Cam test, including settings, vehicles, respawn and aiming behavior.
11. Verify recoil, sound, magazines and attachment compatibility for each approved weapon.
12. Establish multiplayer performance acceptance criteria for blood, smoke, drones and other effects.
13. Evaluate Maxxpro turret limitations and FritV3 material/editor/vehicle warnings; no fixes made here.
14. Decide reproducible VT4 scratch dependency setup and team catalog/spawn allocation; inspect Equipped variants.
15. Decide FTA's role relative to NML progression, XP thresholds/rewards/reset policy and rank communication with Minimal HUD.
16. Set map/ATAK visibility, spawn protection, grenade limits and vehicle ownership/persistence policies.
17. Configure admin authority and GM Tools/Discord feed ownership; Discord Admin Stats remains missing locally.
18. Resolve recorded RTS Protocol public-server and SW-Reap-IR server authorization requirements before deployment; no permission obtained is claimed.
19. Find Bacon's exact editor action/component and determine whether it mirrors an overwrite list or another catalog.
20. Verify NML server enforcement through Bacon and other acquisition routes; no whitelist test exists.
21. Decide whether Bacon Arsenal Only/IRBA or face selection are needed; they remain optional/deferred.
22. Assign Myrove evaluation and resolve its dependency names, transitive closure, completeness and compatibility.
23. Establish dedicated-server installation/path and a two-client test setup.
24. Approve production integration scope/owner/branch and dependency promotion before any `.gproj` or server-list edit.
25. Explicitly approve Stage 5.3 before implementation. Follow accepted vanilla-first ADR 0005; do not infer an architecture change from this research.

No production dependency, addon, server setting, accepted ADR or third-party resource is changed by publishing this handoff. Review and merge remain with Weston; contributor work is not self-merged.
