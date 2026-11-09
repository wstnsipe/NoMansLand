# NML_Visuals v2 — Stage 0 research and audit

- **Status:** research only. No addon behaviour changed. Stage 1 needs the lead's approval.
- **Date:** 2026-11-09. **Branch:** `feat/visuals-v2`. **Baseline:** NML_Visuals v1.0.0 (PR #14, [ADR 0007](../../adr/0007-nml-visuals.md)).
- **Problem report (FRIT):** "Rendering performance degradation while NML_Visuals was active on the populated live server with the full production mod stack. Solo/local testing did not reproduce the issue." There are no benchmark numbers. The working symptom is an FPS drop while ON. Stage 1 must still reproduce it before a root cause is assigned.

## 0. Sources and evidence levels

| Source | Used for | Evidence level |
|---|---|---|
| v1 source (`addons/NML_Visuals`) | Performance audit | Confirmed |
| Client logs under `.local/client*` (diag sessions) | Re-apply churn counts | Confirmed (local diag only) |
| Enfusion MCP `api_search`, `wiki_search`, `game_browse`, `asset_search` | Vanilla hooks, post-process (PP) slots | Confirmed API names; vanilla script bodies unreadable (packed) |
| GitHub: PR #14, issues, [acemod/ACE-Anvil](https://github.com/acemod/ACE-Anvil) (GPL-2.0), [burn0ut7/GC-Suppression](https://github.com/burn0ut7/GC-Suppression) (no LICENSE file; Workshop says APL) | ACE and GC behaviour | Confirmed from source, **current repository head**, not the production version |
| Workshop pages (77 fetched) | Metadata classification | Author claims |
| **Local Arma Workshop cache**, read-only | Versions and pak file tables for all 184 production mods | File paths confirmed; content not extracted |
| Production list "Custom #784" (184 mods, supplied by the lead) | Inventory | Authoritative for names and IDs; **no versions** |

### Version policy

No authoritative production versions exist. The live server config is not in the repo (`server/` holds templates only) and no server export was available. Every mod is therefore **VERSION UNVERIFIED**.

- "Local" means the version in the lead's local Arma Workshop cache. It was probably downloaded by joining the server, but that is not verified.
- "Current" means the CURRENT WORKSHOP VERSION, NOT VERIFIED PRODUCTION VERSION.
- For 26 high-risk mods a stopped download job left the current version in `.local/research/workshop-current`. Local and current match for 22 of them. Local is **older** for 4: Attachment Framework-Core (1.0.19 vs 1.0.20), Corns Orlan-10 (0.2.4 vs 0.2.5), S&D Drone Systems (0.3.0 vs 0.3.4) and SH-Garbage-Collector (8.8.0 vs 9.0.0). The Workshop page also shows No Mans Land at 1.0.93 against local 1.0.92. Older local copies fit a cache filled by a server that pins versions, but a cache that simply has not updated explains them equally well. Local ≈ production is **not established**.

### Inspection method

- **All 184 production mods are present in the local cache.** Every inspection used those existing copies, read-only. Nothing in the cache was moved, modified or deleted.
- **Newly downloaded:** none were needed. A first download job, started before the cache was checked, was stopped. It left current-version copies of 26 mods (plus dependencies) in the gitignored `.local/research/workshop-current/`. They were used only for the version comparison above, and can be deleted on the lead's word.
- **Pak inspection was limited to the file table** (paths and sizes) of each `data.pak` (PAC1 `FILE` chunk), read with a throwaway script. **No file content was decompressed or extracted.** Pak content is compressed, and several licences restrict extraction: Phaseline forbids it, and BWICamera Fix, CERTII, Stun Grenade and PVS-27 have custom or restrictive terms. Whether reading content for compatibility research is allowed is an **open question for the lead**. Nothing here concludes that it is.
- **Limitation:** a file path shows that a mod *overrides* or *ships* a resource. It does not show which PP priority a script uses or when it writes. Those are the runtime unknowns in section 11.

### Facts about the production list

- **NML_Visuals (81E04280BF506AF2) is not in Custom #784.** The list therefore describes the stack without v1, presumably after it was removed. v1 must be re-added for measurement.
- The terrains are **Bakhmut City** and **Ovruch_Fixed**. Myrove is not in the list, so ADR 0007's "Myrove + Warfare-Colormod baseline" is not the production baseline.
- BWI Camera Adjustments 2.0 is listed with a truncated ID (`66489CC231155`). The cache and Workshop resolve it to `66489CC231155B78`.
- `69D92CDA0AE467E1` and `69F419849D110324` are now named SH-Item-Limiter and SH-Garbage-Collector on the Workshop.

---

## 1. V1 performance audit

Code paths are in `addons/NML_Visuals/Scripts/Game/NML/Visuals/NML_VisualsManager.c`.

| Cadence | Work |
|---|---|
| **Every frame (GPU)** | PP slot 10 ColorGrading LUT (:453), slot 11 Colors saturation trim (:456), slot 13 ChromAber fringe (:461, if lens on). 3 stacked full-screen alpha `ImageWidget`s under the HUD root: Vignette 512², Grime and GrimeHeavy 1024×576. **Grime layers stay visible at opacity 0** (night, LOW/AUTHENTIC), so they may still be drawn. |
| **Every 100 ms** (call-queue repeat, :117) | Panel check (`GetTopMenu`), world check, `UpdateOverlay` (HUD manager and root lookup), camera id, controlled entity, `UpdateParams`: 2× `GetOrLoadMaterial`, `SetParam` on delta only, **3× unconditional `SetOpacity`**. Everything except params and weather **also runs while OFF**. |
| **Every 500 ms** | `SampleWeather` (`TryGetCompleteLocalWeather` at the camera, sun times). |
| **On events** | `Apply` = clear 3 slots + set 3 slots + log line. Triggered by: dirty flag, camera-id change, controlled-entity change, low-light LUT flip. `OnUserSettingsChanged` fires on **any** game-settings save (not only ours) and forces a full re-apply. |

Evidence from local diag logs: 50–63 `Apply` calls per session. The video settings menu caused a re-apply every 100 ms. Many applies followed "Save data transaction to 'settings'" with no NML input.

### Ranked risks

| Rank | Risk | Kind |
|---|---|---|
| **CRITICAL (compatibility)** | **PP slot 13 shared with GC Suppression.** GC (`GC_SuppressionScreenEffect`) sets 12 RadialBlur / **13 Colors** / 14 ChromAber in `DisplayStartDraw`. That is confirmed at the GC repository **head**; it is unverified for the local 1.7.2. v1 clears and rewrites 13 as ChromAber on every `Apply`. The last writer wins: v1 can delete GC's suppression grade, or GC can delete v1's fringe. This is a **correctness conflict** that exists only with the production stack. As a cause of a *sustained* FPS drop it is unproven: writes happen only on events, so at most it explains hitches or a broken look. | Render correctness |
| **HIGH** | **Clear-then-set `Apply` churn.** Triggered by:<br>- camera switches;<br>- controlled-entity changes;<br>- any settings save;<br>- every world load (`OnWorldPostProcess` → `Start()` sets dirty; unknown whether preview or render-target worlds also fire it);<br>- each low-light LUT flip.<br>The production stack multiplies these triggers: 7 mods ship camera-handler scripts, 3 drone mods switch cameras or controlled entities, many PiP sights exist, and several mods have settings. If chain rebuilds happen they show up as hitches, and v1 clears slots that other mods may share. | Hitch / conflict |
| **HIGH (hypothesis A)** | **3 persistent full-screen PP passes** on top of vanilla 5–8, ACE 7, GC 12–14, EESuppression's blur and CA, and NVG chains. These cost FPS only if the client is **GPU-bound**. That is plausible on dense terrain (Bakhmut City) with lasers, smoke and drones. | GPU |
| **HIGH (hypothesis A)** | **3 full-screen alpha overlays**, including grime drawn at opacity 0. They join GC's vignette and flinch widgets and ACE's flash widgets on the same screen. | GPU fill rate |
| **HIGH (hypothesis B)** | Populated Reforger clients are often **CPU-bound**, in which case extra passes cost little and the drop comes from **hitches or script work** (rows above and below) or from causes outside NML. **Hypotheses A and B are ranked only after §12 measures whether frames are GPU- or CPU-bound.** | Unknown |
| MEDIUM | 3× `SetOpacity` per poll regardless of change (may dirty UI redraw). While weather or time is changing, the `SampleWeather` blend moves saturation enough to call `SetParam` about every 500 ms. | Script/UI |
| MEDIUM | Work while OFF: the 10 Hz poll (including `NML_VisualsPanel.Update` → `GetTopMenu`) and widget lookups. | Script |
| LOW | HUD manager/root and material lookups per poll (cached by the engine); weather at 2 Hz; the timer itself. | Script |

Player count: v1 iterates no entities and replicates nothing, so its **script** cost does not scale with players. What scales is the load around it (GPU or CPU) and the number of camera and controlled-entity switches. **No measurement exists yet. Nothing in this table is proven.**

**Removable:** slot 11 (bake saturation into LUT variants), persistent ChromAber, GrimeHeavy as a separate layer, the always-on poll, the unconditional clear in `Apply`.
**Cacheable:** HUD root, materials, last opacity.
**Event-driven:** settings (filter to our module), respawn, camera handler, menus.

## 2. Compatibility matrix — high-risk mods

Ratings: GREEN = **no overlap found** in the file table or metadata. A file table cannot show scripts that set PP effects with vanilla materials, so GREEN is not proof. YELLOW = overlap but likely compatible; RED = direct conflict risk; INTEGRATE = reuse its state.

Local versions come from the local cache (VERSION UNVERIFIED). "DPC" = overrides `Prefabs/Characters/Core/DefaultPlayerController.et`. "SE layout" = overrides `UI/layouts/HUD/ScreenEffects/ScreenEffects.layout`.

### Post-process / screen effects

| Mod | Local ver. | Evidence (file table unless noted) | Rating |
|---|---|---|---|
| **GC Suppression** 684CE8AA3B1D6573 | 1.7.2 | DPC, SE layout, `GC_SuppressionScreenEffect.c`, own Blur/ChromAber/Color PP materials; source: slots 12/13/14, `GC_SuppressionSystem.GetInstance().GetAmount()` | **RED → INTEGRATE** |
| **ACE Medical Core/Circulation/Breathing/Hitzones**, ACE Core, ACE Compass | 1.5.40–41 | DPC (×5: Core, Medical Core, Circulation, Breathing, Compass), SE layout (Medical Core), `ACE_Medical_PostProcessPainScreenEffectMode.c`, `ACE_Medical_ChromAber/RadialBlur.emat`, modded `SCR_BleedingScreenEffect`; ACE Core mods `SCR_CharacterCameraHandlerComponent`, `SCR_PlayerController`, `ArmaReforgerScripted`, `TimeAndWeatherManagerEntity`; source: slot 7, cardiac-arrest blackout | **INTEGRATE** (vitals via `GetVitalStateID`, pain via `ACE_Medical_IsInPain`) |
| **EESuppression** 665C00EFCCAE1AB3 | 1.0.5 | `UI/SuppressionBlur.emat`, `UI/SuppressionChromAber.emat`, 1 script; priorities unknown | **YELLOW** (blur/CA stacking; slots unknown) |
| **Smokeable Smokes** 65D050C86106E5BC | 3.2.0 | **SE layout** (third override), 54 scripts, 27 effect-related files | **YELLOW** (layout fight with ACE/GC) |
| **Warfare Colormod** 65A2EA40DC9E632A | 1.0.13 | overrides `UI/Materials/ScreenEffects_ColorPP.emat` (vanilla slot 5) | INTEGRATE (baseline) |
| **No Mans Land** 69D06CC450E23BEE | 1.0.92 (current 1.0.93) | **also overrides `ScreenEffects_ColorPP.emat`** | YELLOW (baseline owner unknown) |
| **Bakhmut City** 65F173E7EA1B5A98 | 0.0.73 | **overrides `ScreenEffects_ColorPP.emat` and `ScreenEffects_GaussBlurPP.emat`** (vanilla damage blur, slot 7) | YELLOW (baseline, damage blur changed) |
| **Ovruch_Fixed** 6AB54487753939BB | 1.0.3 | **overrides `ScreenEffects_ColorPP.emat`**; requires Warfare Colormod | YELLOW (baseline) |
| Stun Grenade 59EAA899751805DF | 1.3.0 | 5 scripts, 3 effect-related files; restrictive licence | YELLOW (concussion overlap) |
| Improved Blood Effect / Deluxe | 1.5.9 / 1.0.3 | no screen-effect files matched | GREEN (screen); world blood only |
| 2-7 Suppressor Overheating 66B073D763F66862 | 1.1.7 | 14 scripts; page: smoke/puff effects **only on a server** | YELLOW (**perf factor that appears only on a server**) |
| CIE Tracked Core 68CD35053063C6D5 | 0.0.121 | `LTM_CharacterCameraHandlerComponent.c`, `LTM_CrewConcussionBlackout.layout`, triplex and damage overlays | YELLOW (crew concussion overlaps ours) |

### Camera

| Mod | Evidence | Rating |
|---|---|---|
| NV-System 64722DADC53CB75E (required by FORTEX) | DPC, `NVS_CharacterCameraHandlerComponent.c`, `NVS_PostProcessEffect.c`, `NVS_FadeEffect.c` (screen effect), `NVS_2DPiPSightsComponent.c`, IR laser/strobe scripts | **YELLOW** (suspend NML grade while NVG is on) |
| TacticalAnimationOverhaul TEST, BWI Camera Adj. 2.0, Zeliks Character, RTS Protocol | `*_CharacterCameraHandlerComponent.c` each | YELLOW (camera handler chain; our hooks must be read-only) |
| Best Body Cam 64FD308B60F7B687 | no camera/PP files matched in the file table | GREEN (low confidence) |
| Immersive Head Movement, Better Weapon Immersion 2.8, Aiming Deadzone (DPC), BWICamera Fix, TacPro Recon 5 | animation / camera motion / DPC only | GREEN |

### NVG / thermal / optics (picture-in-picture)

| Mod | Evidence | Rating |
|---|---|---|
| Phaseline NVG Systems 6F417A32E17A2178 | DPC, `GMS_ZDT_NightVision.c`, `GMS_ZDT_NVPostProcessBaseline.c`, own HDR/Grain/OpticalBlur/DistanceSoftening/GodRays/phosphor/thermal PP materials | **YELLOW** (heavy PP chain; suspend NML while active; licence forbids patching) |
| CIEThermal 68F8A813D7EF1E30 | 11 scripts: `NightVisionBallisticPIPSightsComponent`, `NightVisionThermal2D*`, thermal HUD layouts | YELLOW (PiP) |
| PVS-27 Thermal Clip-On 6A6380143AD78BBE | own `Common/Postprocess/PVS27TH/*` (HDR, Colors, gain) | YELLOW |
| Rayzis Optics 628F720BC527C143 | `RAYZI_2DPIPNightVision.c` | YELLOW (PiP) |
| FORTEX Framework 66C2ED22F25B05C9, Pronto Hub 631A9C6C92283C44 | `FORTEX_NVSNightVisionHooks.c`; `Pronto_NightVisionComponent.c`, `Pronto_EarplugsOverlay.layout` | YELLOW |
| CIE T72B, ATGM, TALOS, BMP-2 Modernized | vehicle thermal sight layouts/materials (26 matches in TALOS) | YELLOW (vehicle sights) |
| Advanced Zeroing System | DPC, `WeaponInfo.layout` override | GREEN |

### HUD / UI

| Mod | Evidence | Rating |
|---|---|---|
| CERTII-Minimal-HUD 69CEF7D584AFD1FA | DPC, 8 scripts | YELLOW (HUD root/z-order) |
| Kova x ATAK 6A49417788E13618 | DPC, 147 scripts, full-screen menu | YELLOW (overlays must not cover its menu) |
| GSC X TWS Earplugs 6A789A8EE7260C08 | overrides `DefaultPlayerControllerMP.et`, own HUD | YELLOW + INTEGRATE (earplug state → deafness) |
| Casualz Admin Tools | spectator cameras, ESP overlays | YELLOW (admin camera) |
| Player Map Markers (modded `SCR_PlayerController`), Where Am I, ATAK compat ×2, Disable Enemy Proximity VON UI (DPC), CM-SquadRadioSound (VON layout), Realism Overhaul Radios | map/VON UI | GREEN |

### Drones / special cameras

| Mod | Evidence | Rating |
|---|---|---|
| Realistic Combat Drones 65AD60E204191D37 | modded `SCR_PlayerController` (`SAL_`), `ScreenEffects_ChromAberrPPDrone.emat`, DJI/FPV HUD layouts | **YELLOW** (controlled-entity/camera switch → v1 re-apply; NML over drone feed) |
| S&D Drone Systems 6A89645B840FD4EE | 193 scripts, 44 matches | YELLOW (same) |
| Corns Orlan-10 B1738694C84A4F20 | page: 1920×1080 render-target camera feed with NV/thermal modes | YELLOW (second scene render; perf) |
| Vehicles (Ukrainian War Vehicles, RUVicExpansion, VT4, M1224 + fix, BMP/T-72 packs) | M1224 fix touches the gunner third-person camera | GREEN/YELLOW (camera switch) |

### Explosives / damage sources

Extra Grenades, Mark 1 Thermobaric, ACE Backblast, ACE Cook-Off, Corns Booby Traps, Shotguns GO BOOM: GREEN for the camera. They are event sources (blast damage reaches `GetOnDamageEffectAdded`). Stun Grenade: YELLOW (see above).

### Shared-resource collision summary (from file tables)

| Vanilla resource | Overridden by | Consequence |
|---|---|---|
| `Prefabs/Characters/Core/DefaultPlayerController.et` | ACE Core, Medical Core, Circulation, Breathing, Compass; GC Suppression; NV-System; Phaseline NVG; Advanced Zeroing; Aiming Deadzone; Attachment Framework-Core; Bacon Loadout Editor; Bon Action Animations; Disable Enemy Proximity VON UI; Kova x ATAK; CERTII-Minimal-HUD; RIS Laser Attachments (17) | **v2 must not add another override** |
| `UI/layouts/HUD/ScreenEffects/ScreenEffects.layout` | ACE Medical Core, GC Suppression, Smokeable Smokes | 3-way, last-loaded wins; v2 must not touch it |
| `UI/Materials/ScreenEffects_ColorPP.emat` (vanilla slot 5) | Warfare Colormod, No Mans Land, Bakhmut City, Ovruch_Fixed | The production colour baseline is whichever loads last, **not** Colormod as ADR 0007 assumed |
| `UI/Materials/ScreenEffects_GaussBlurPP.emat` (slot 7) | Bakhmut City | Vanilla damage blur changed |
| `SCR_CharacterCameraHandlerComponent` (script files) | ACE Core, NV-System, TAO, BWI Cam 2.0, Zeliks, RTS, CIE Tracked Core (LTM) | v2 reads camera state; never mods this class |

## 3. The other ~130 mods (metadata level)

**YELLOW (performance factor under multiplayer load, no camera conflict):**
- Phaseline Modular Rifles (lasers, IR, lights; 1.02 GB), RIS Laser Attachments, Attachment Framework: many players with lasers or lights.
- Bakhmut City (470 MB, dense ruins).
- Shotguns GO BOOM (fire and spark effects).
- SH-Garbage-Collector: controls how many entities stay in the world.

**YELLOW (low or unknown):**
- Fog of War (3 fog types, 2.8 KB).
- GM Tools (Game Master exposure-based night vision).
- Laser Rangefinder, TGZRecoilRealism, Only Launchers 2nd Slot.
- SH-Item-Limiter, TALOS (WIP, no description).
- Frameworks: GRS Dev, SHS Modular, Pronto Hub, Nested Attachments.
- PR_UTILS, TFO Drone Jammer Backpacks.

**GREEN:**
- **Gear and clothing:** RTG Armbands, FORTEX RTG Asset pack, FORTEX Russian Tactical Gear, FORTEX UTG, Minnesinger ×4, MNSGR Ghillies, GRSBEARDS, Flag Patches, GRS Patches, SHS Apparel, SHS-Patches, Civilian Clothing, Darkline Tattoos, Press Gear, BM Gloves, Kitbash, TFO Backpacks & IFAKs, TFO Pre-packed IFAK, LEGBAG, Medical Latex Gloves, Dogtags.
- **Weapons:** AT-4, RGM-40GL-06, HK-G95KA1, MSBS Grot, MCR-Pack, FN SCAR, FORTEX RTW, FORTEX RU Weapon Core, Extended Fortex Taped Mags, Slavic War AK74, Bacon Suppressors, Phaseline Suppressors, MuffsMods, Extra Mines, G3A3, Prontos ×4, Moists L119, Vulkan M, M4 Block II, M17, CIE UAR-10, Nasty Stechkin, PP-19, Nasty SKS V2, Ruger Mark IV, ASH12, SPECTRE SVD, M249 Scope Rails, Bacon Loadout Editor.
- **Vehicles:** Zagoria BMP2, K02-1 ATV, Realistic Anti-Drone Cages.
- **Audio:** Ukraine Radio Chatter, Screams of War, CM-SquadRadioSound (+Addon), SR Sound Mod, Lunacy Audio, SR X Lunacy, Fortex RTW x SRxLunacy, Tactile Sound Enhancement, More Handheld Radios.
- **Props and maps:** CIE Props, Booses Bodies, Snowzin Trenches, Minus Building Pack, Malvian Bushwar, UlupsAssets, RUINS models, Assets for Tools byHeine, WolfsBuildingPack, Rail Enfusion Redux, Railway Generator, Ukraine Trenches, MLEasternEuropeanAssets.
- **Admin and gameplay:** Simple Spawn Protection, FTA Persistent Ranks, Game Master Enhanced, AAO Team Balancer, Server Admin Tools, Rayzi Utils, ACE Carrying, Magazine Repack, Trenches, Captives, Chopping, Compass and Finger, NSW Misc, Bon Action Animations.

No Workshop page names a conflict with camera, PP, HUD or screen-effect mods.

## 4. Known / likely conflicts

1. **Slot 13 (GC Suppression).** Confirmed at the GC repository head; unverified for the local 1.7.2 (§11.3 census).
2. **Shared vanilla resource overrides:** the controller prefab ×17, the screen-effects layout ×3, the slot-5 colour material ×4. v2 must add none of these.
3. **Stacked blur and CA:** vanilla 6/7 (Bakhmut's blur material), ACE 7, GC 12/14, EESuppression's blur/CA, NVG chains. Adding v1's fringe gives a doubled suppression and injury look plus the GPU cost.
4. **PiP / NVG / thermal / drones / vehicle sights:** NML grade and overlays drawn over devices that own the view, plus re-apply on each camera or controlled-entity switch.
5. **Look baseline:** v1 was tuned against Colormod, which was inert on game 1.8.0.13. The production game version is not recorded (§11.7). Production slot 5 is decided by load order among 4 mods, so the v1 look in production is untested.

## 5. Integration opportunities

- **Suppression:** read GC's `GetAmount()` when GC is loaded. NML then adds no suppression blur or CA of its own when GC or EESuppression is loaded.
- **Blood and medical:** vanilla damage invokers by default. With ACE, defer blackout and pain visuals to ACE, and read its vital state only to scale dirt and stress.
- **Deafness / concussion:** earplug state (GSC X TWS, Pronto) could gate an ear-ringing look later.
- Third-party reads go in optional `NML_Compat_<Mod>` addons (CLAUDE.md convention), so NML_Visuals keeps zero hard dependencies.

## 6. Proposed v2 architecture (client only)

- **`NML_CameraStateModel`:** channels of `{ value 0–1, source, accumulate(event, amount), decayPerSec, resetOn[] }`. Pure data and math, so it can be unit-tested. **Channels are added per MVP step**, not all eight up front (no speculative infrastructure). The full set is Dirt, Blood, Dust, Wetness, Suppression, Concussion, Stress and LowLight.
- **`NML_CameraSignalHub`:** tracks the **player's own character separately from the controlled entity**. Drones and vehicles change the controlled entity, but damage to the character must still count. It handles a null character (GM, spectator, between lives) and binds only in the game world. One throttled sampler covers inputs that have no events.
- **`NML_CameraRenderer`:** maps channels to output.
  - **1 persistent PP pass:** slot 10 LUT, with day/low-light/rain variants and saturation baked in.
  - **≤1 transient pass:** concussion fringe/blur, only while > 0. **Conditional on §11.2.** If set/clear rebuilds the chain, keep the pass resident at a neutral value, or drop it. The slot is chosen after the runtime census, not 12–14.
  - **One overlay container:** ≤2 visible full-screen images, `SetVisible(false)` at 0, opacity pushed only on change.
- **Suspend rules:** non-character camera, drone or vehicle-sight camera, GM/editor, and menus **except the pause menu**, which hosts the preview panel. NVG and thermal suspension is **best effort, through compat addons only**: no vanilla NVG hook exists (§7), and Phaseline forbids patching.
- **Write discipline:** set a slot only when its material changes; never clear a slot we did not set; no clear-then-set. No API to read a slot back was found. Re-assert therefore runs only on defined triggers (respawn, camera-id change), and being overwritten in between is an accepted risk. Clearing on OFF can still remove another mod's effect if it reused our slot; the census in §11.3 picks a slot nobody else uses.
- **OFF ≈ zero:** no timer, no character subscriptions, no widgets; slots cleared once. The only allowed leftovers are the settings listener and pause-menu panel detection, which are needed to turn it back on.
- **No controller-prefab override and no `ScreenEffects.layout` override**, which rules out registering as an `SCR_BaseScreenEffect` through the prefab. Revisit only if a script-side registration path is confirmed.
- **Medical:** visuals only *read* vanilla damage state. The lead should confirm that ADR 0004 (`NML_MedicalService`) does not apply to a cosmetic addon that depends on the base game only.

## 7. Event-driven vs polling map

| Trigger | Source (confirmed API names) | Mode |
|---|---|---|
| Damage / blast | `SCR_CharacterDamageManagerComponent.GetOnDamage`, `GetOnDamageEffectAdded/Removed` (`SCR_ExplosionDamageEffect`) | Event |
| Bleeding amount | `IsBleeding`, `SCR_CharacterBloodHitZone.GetTotalBleedingAmount` | Event start, then 2 Hz while bleeding |
| Unconscious / death | `OnConsciousnessChanged`, `GetOnPlayerDeath`, life-state invoker | Event |
| Respawn | `SCR_PlayerController` controlled-entity changed | Event (reset) |
| Water | `SCR_CharacterBuoyancyComponent.GetOnWaterEnter/Exit` | Event |
| Third person | `SCR_CharacterCameraHandlerComponent.GetThirdPersonSwitchInvoker` | Event |
| Vehicle | `SCR_CompartmentAccessComponent.GetOnCompartmentEntered/Left` | Event |
| Menus | `SCR_MenuHelper.GetOnMenuOpen/Close` | Event |
| Settings | settings invoker, filtered to our module values | Event |
| Stance (prone dirt) | `CharacterControllerComponent.GetStance` | 2–4 Hz |
| Rain / wetness / fog | `TryGetCompleteLocalWeather` → `LocalWeatherSituation` | 1 Hz |
| Time / low light | `GetTimeOfTheDay`, sunrise/sunset, `IsNightHour` | 1 Hz or slower |
| ADS / PiP / camera id | `IsWeaponADS`, `SCR_2DPIPSightsComponent.IsPIPActive`, camera id | 2–4 Hz |
| Suppression | GC `GetAmount()` (compat addon) | 4 Hz while > 0 |
| Explosion without damage, smoke density, flashbang, NVG on/off | **No vanilla client hook found** | Out of MVP / compat only |

## 8. Performance budgets

- ≤1 persistent + ≤1 transient PP pass; no permanent blur or CA.
- ≤2 visible full-screen overlay images, and 0 when all channels are 0.
- Script ≤0.05 ms per frame on average; no per-frame callback while all channels are at rest.
- Weather and time at 1 Hz; fallback sampler at 2–4 Hz; a 10 Hz animation tick only while a channel is changing.
- No allocations in polls; materials, widgets and components cached.
- OFF: no timers, widgets or subscriptions.
- **Release gate, measured with the production stack:** ON vs OFF ≤ ~0.3 ms GPU frame time and ≤2 % average FPS, with no new 1 %-low spikes.

## 9. MVP order (each step gated)

1. **v1.1 measurement build and hotfixes:** move off slot 13, no clear-then-set, filter settings changes, hide widgets at opacity 0, idle when OFF. Its purpose is to prove the root cause.
2. State model, hub and renderer skeleton; LowLight and Wetness (poll only, low risk).
3. Blood and Dirt (damage and stance events); reset on respawn.
4. Concussion (blast damage effect) on the transient pass.
5. Suppression through the GC compat addon; Stress derived from the other channels.
6. Presets as intensity multipliers: Performance, Authentic, Immersive, Custom.

## 10. What NOT to implement

- Overrides of the controller prefab, `ScreenEffects.layout` or `ScreenEffects_*PP.emat`.
- Modded vanilla screen-effect or camera-handler classes.
- Our own suppression detection when GC or EESuppression is loaded.
- Projectile or explosion scans, or per-frame entity queries.
- Permanent blur, CA or grain.
- Any brightness gain at night.
- Effects on PiP, drone or vehicle-sight cameras. NVG and thermal views are avoided on a best-effort basis through compat detection (see §6).
- Anything replicated.

## 11. Unknowns needing Workbench / runtime tests

1. GPU cost per PP pass and per full-screen overlay; whether widgets at opacity 0 are still drawn.
2. Whether `SetCameraPostProcessEffect` rebuilds the chain (a hitch), and whether a type swap at the same priority does.
3. **PP slot census with the production stack.** Log every non-empty priority 0–19 on the player camera at idle, suppressed, injured, NVG on, PiP, drone and vehicle sight. This covers EESuppression, NV-System, Phaseline, PVS-27, Stun Grenade, Smokeable Smokes, drone mods and `SCR_RegenerationScreenEffect.CHROM_ABER_PRIORITY`.
4. Which `ScreenEffects.layout` (ACE / GC / Smokeable) and which `ScreenEffects_ColorPP.emat` (Colormod / NML / Bakhmut / Ovruch) win in production load order; whether GC's widgets still resolve.
5. Camera-id behaviour for PiP, NVG, drones, vehicles and GM.
6. What triggers `UserSettingsChanged` on the production stack, and how often.
7. Exact production mod versions and the production **game version**, from a live server config export and log.
8. How often `OnWorldPostProcess` → `Start()` fires (preview or render-target worlds?).

## 12. Measurement plan (reproduce before assigning cause)

Every run compares **OFF vs LOW / AUTHENTIC / HEAVY / EXTREME**. It records:
- average FPS, 1 % lows, and the count of `[NML] Visuals: applied` log lines;
- a frame-time trace with **GPU vs CPU** time from an external tool (PresentMon) or the engine profiler, since a script logger cannot separate the two.

Live-server noise is far above a 2 % gate. ON and OFF are therefore compared by **toggling in alternating intervals at the same spot** (a release-build version of `-nmlVisualsCycle`), not across separate sessions. Mods cannot be removed from the live server, so mod bisects happen only in local runs (steps 2–3, 5).

1. **Solo local baseline:** diag client, vanilla terrain, NML_Visuals only.
2. **Solo with the production stack:** Bakhmut City and Ovruch_Fixed, all 184 mods from the local cache.
3. **Low-population multiplayer:** local server and 2–3 clients (`tools/server-dev.ps1`, `client-dev.ps1`), production stack.
4. **Higher population / load (FRIT):** production-like server, combat load: smoke, grenades, suppression, bleeding, vehicles, drones, NVG, lasers.
5. **Bisect** at the worst condition:
   - v1 with LUT only, then + trim, + fringe, + overlays.
   - With and without GC Suppression, EESuppression, NVG mods and drone mods.
   - With and without 2-7 Suppressor Overheating (its effects run only on a server).

## 13. Proposed Stage 1 (separate approval)

1. Build v1.1 as a measurement build: a slot-census and apply-count logger, an ON/OFF interval cycler, and the MVP-1 hotfixes. Each piece sits behind **CLI parameters that work in release builds**; today's `-nmlVisualsOnly` and `-nmlVisualsCycle` are compiled only in diag builds, and players run release builds.
2. Run measurement plan steps 1–3 locally. Step 4 needs the build on a live-like server. That requires a **lead-approved Workshop publish** (e.g. an unlisted test item) and FRIT running a scripted test.
3. Write ADR 0008 (v2 architecture), informed by the measured root cause.

## 14. Tooling used

- **Used:**
  - Enfusion MCP (API and wiki search; vanilla pak scripts unreadable: "incorrect header check").
  - GitHub MCP (PR #14, issues, ACE and GC source).
  - WebSearch and WebFetch (77 Workshop pages).
  - Parallel read-only research subagents.
  - A throwaway read-only PAC1 file-table lister.
  - The ECC `architect` agent reviewed this document. Its 4 blocking and 11 should-fix findings are folded in: GPU vs CPU hypotheses, the slot-13 correctness vs performance split, NVG suspension as best effort, a release-build measurement path, hub character tracking, write-discipline limits and incremental channels.
- **Not useful:** graphify returns only file-level nodes for Enforce Script, and its graph was stale. `workshop_info` reads local `.gproj` files only.
- **Not added:** no new plugins or MCPs. A pak content extractor would answer the slot unknowns, but it depends on the licence decision above.
