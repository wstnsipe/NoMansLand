# FRIT local movement and weapon immersion selection

This records the member-directed local `fritledangv1` experiment, not an approved NML production dependency change. Owner: FRIT/Codex. No production files or third-party assets are included.

## Current selection

| Mod | Workshop ID / project GUID | Installed version | Direct dependencies | State |
|---|---|---|---|---|
| Better Weapon Immersion 2.8 | `5A7B79D8A910A4D1` | 2.8.1 | Base game `58D0FB3206B6F859` | Selected locally; runtime checks pending |
| Immersive Head Movement | `6622D3D1E5A3809D` | 1.5.1 | Base game `58D0FB3206B6F859` | Selected locally; runtime checks pending |
| CERTII-SimulatedMovement | `69B252D91D8172C2` | Previously 7.0.10 | Removed from active local selection | Download retained |
| Better Weapon Immersion ADSs | `65F76D9612BE5C94` | Not selected | Not evaluated for this experiment | Explicitly left out by member direction |

The two additions have no additional Workshop transitive prerequisites. TAO, Best Body Cam, CERTII EarPlugs, CERTII Minimal HUD and CERTII Grenade Limiter remain selected. No arsenal or rank-lock changes were made.

## Verification and compatibility

Downloaded package project files and ServerData metadata were read directly. Local addon dependency selection, build manifest, and local test package lock were updated. Static validation passed: 118 external packages, complete selected dependency closure, 140 parsed resources and 162 local resource references. Western arsenal remains 1,282 entries; Eastern remains 1,092.

Package resource-path comparison found 13 weapon prefab paths shared by Better Weapon Immersion and TAO: M9, PM, RPG7, M249, M60, PKM, RPK74, UK59, AK74, M21, M16A2, SVD and VZ58 base prefabs. This establishes overlapping overrides, not a confirmed incompatibility or a successful merge. Neither addition shared resource paths with Best Body Cam; Immersive Head Movement shared none with TAO. Script behavior can still interact without shared paths.

Fresh Workbench loading, compilation, ADS/recoil behavior, TAO animation retention, bodycam/head movement interaction, and multiplayer verification remain pending. The Workbench API connection was unavailable during this change. Static validation is not gameplay verification.

## Handoff to Weston/Claude

### Camera additions requested after the initial selection

The member subsequently requested both downloaded camera packages. BWI Camera Adjustments 2.0 (`66489CC231155B78`, same project GUID) version 2.0.3 and BWICamera Fix (`6A770A0F54F1AEDE`, same project GUID) version 1.0.3 are now selected in the local addon and test package lock. Adjustments directly requires only the base game. Fix directly requires base game, BWI 2.8, Camera Adjustments and TAO; all prerequisites are already present and their closure adds no other packages.

Inspection of the external fix script shows two intended fixes: clamp the controller control-update time slice to at least 0.0001 to avoid zero-time divisions, and provide TAO deadzone/freelook settings callback methods. These are observed implementation intentions, not demonstrated runtime fixes. Camera Adjustments contains weapon aim modifiers and camera/controller scripts, including ADS sway, weapon inertia and deadzone settings. No third-party code is reproduced here.

Static validation after both additions passed: 120 external packages, complete dependency closure, 141 parsed resources and 162 checked local references. Arsenal counts are unchanged. Both projects are registered locally. Fresh Workbench compile and gameplay testing remain pending; test ADS, firing, freelook, vehicle camera and settings changes before production consideration.

Ready: exact installed versions, dependency identities, member-directed selection and overlap findings. Incomplete: runtime compatibility and multiplayer testing. Next: restart Workbench and compare ADS, firing and animations using the shared vanilla weapons and selected mod weapons. Do not integrate these candidates into NML production yet.

## Remaining Weston decisions

1. After runtime testing, decide whether either candidate belongs in the NML production dependency set.
2. If TAO/BWI overrides lose required behavior, decide the desired weapon behavior before any compatibility implementation.

This supplements the earlier candidate/inventory/arsenal research only for this local movement experiment; it does not replace the still-pending full item-level arsenal handoff.
