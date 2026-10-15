# ADR 0007: NML_Visuals — optional client-side combat-footage look

- **Status:** Accepted (2026-10-14); the lead resolved open items 7 and 8 (see "Decisions resolved by the lead"). **v1.0.0 approved for release (2026-10-15)** after visual sign-off; see "Release v1.0.0" at the end, which supersedes the prototype-era preset table, slot plan and grain/softness plans above.

## Context

The lead wants an optional visual mod that makes the game look like modern helmet-cam / GoPro combat footage for the local player: muted, slightly dirty, imperfect digital video, believable rather than cinematic. It must not change gameplay, must not touch any existing NML addon or system, and must not give a visibility advantage (especially at night).

Research was read-only: Enfusion MCP `api_search` / `wiki_search` / `asset_search` / `game_browse`, BI's published Enfusion and Arma Reforger script API docs, the Workbench MCP bridge, and the Warfare-Colormod package downloaded into the gitignored `.local/workshop` with `tools/workshop-sync.ps1 -ModListJson` (no registry change). Eight reference screenshots from the lead (real Ukraine-war helmet-cam footage plus one Reforger capture) define the target.

### Target look (from the screenshots)

- Clipped skies and highlights (small sensor dynamic range), no soft film roll-off, almost no bloom.
- Lifted, milky shadows (dust/haze, gamma lift); dark ground stays dark grey, never black.
- Muted, olive greens; neutral-to-cool white balance; **no warm/orange push**, no orange/teal.
- Fine digital texture (shadow noise, compression softness), faint corner falloff, mild edge fringing.
- Wide-angle barrel bend and head-roll — not reproducible (see Limitations).

### Engine mechanism

- `BaseWorld.SetCameraPostProcessEffect(int cam, int priority, PostProcessEffectType type, string materialPath)` sets a post-process effect on a camera; an empty material clears the slot. **Priorities 0–19 (max 20 effects); lower renders earlier.** Camera id: `BaseWorld.GetCurrentCameraId()`. Local render state only — nothing replicates.
- Runtime tuning: `Material.GetOrLoadMaterial()` + `Material.SetParam()` / `ResetParam()`.
- `PostProcessEffectType` (BI API docs; material class = enum name + `Effect`): None, UnderWater, SSAO, DepthOfField, HBAO, RotBlur, GodRays, Rain, **FilmGrain**, RadialBlur, **ChromAber**, WetDistort, DynamicBlur, **ColorGrading**, **Colors**, HDR, SMAA, FXAA, Median, SunMask, **GaussFilter**, SSR, DepthOfFieldBokeh, ThermalImaging. There is **no** vignette, sharpen or lens-distortion type.
- Vanilla slots in use (BI API docs):

  | Priority | Type | Owner | Material |
  |---|---|---|---|
  | 5 | Colors | `SCR_DesaturationEffect.COLORS_PP_PRIORITY` (bleeding desaturation) | `{7C202A913EB8B1A9}UI/Materials/ScreenEffects_ColorPP.emat` |
  | 6 | RadialBlur | `SCR_StaminaBlurEffect.RADIAL_BLUR_PRIORITY` | `ScreenEffects_BlurPP.emat` |
  | 7 | GaussFilter | `SCR_DamageBlurEffect.GAUSS_BLUR_PRIORITY` | `ScreenEffects_GaussBlurPP.emat` |
  | 8 | DepthOfField | `SCR_DepthOfFieldEffect.DEPTH_OF_FIELD_PRIORITY` | `ScreenEffects_DepthOfField(Bokeh)PP.emat` |

- Vanilla precedent: Armavision's "Cold War" filter combines Colors + ChromAber + FilmGrain materials (`UI/Materials/Editor/ColdWar/*.emat`, plus `UI/Materials/Editor/FilmGrain.emat`, `ChromAber.emat`, `Colors.emat`). Vanilla vignettes are HUD overlays (`UI/layouts/Editor/Overlays/Vignettes/*`, suppression textures).
- User settings: subclass `ModuleGameSettings`, read/write through `GetGame().GetGameUserSettings().GetModule(...)`, notify with `GetGame().UserSettingsChanged()`, persist with `GetGame().SaveUserSettings()`, listen via `ArmaReforgerScripted.OnUserSettingsChangedInvoker()`. Stored in the player's own profile.
- Settings-menu tabs are defined in vanilla `UI/layouts/Menus/SettingsMenu/SettingsSuperMenu.layout`; adding a tab means overriding that layout (last-loaded mod wins).

### Warfare-Colormod (65A2EA40DC9E632A, v1.0.13) — the Myrove baseline

Required by Myrove (ADR 0006), so every NML player on Myrove has it. Its package (project id `OBleach`) is a single override of vanilla **`{7C202A913EB8B1A9}UI/Materials/ScreenEffects_ColorPP.emat`** (`ColorsEffect`), no scripts:

| Parameter | Value | Effect |
|---|---|---|
| `Saturation` | 0.515 | strong desaturation |
| `ColorizationColor` | 1, 0.959, 0.638 | warm yellow/sepia cast |
| `Offset` | −0.005 | slight black crush |
| `DesaturationWeights` | −0.001, 0.587, 0.114 | red dropped from luma → reds darken |
| `OverlayColor` / `OverlayFactor` | 0.374 0 0.644 / 0.005 | negligible |

It therefore sits in vanilla's priority-5 Colors slot. Its muting already exceeds the target; its warmth and black crush work against it.

## Decision

### Addon

- New standalone addon **`NML_Visuals`**: depends on the base game only — **not** on NML_Core, NML_Content or any scenario. Optional, client-side; servers do not load it and nothing is replicated.
- Layout (only what the prototype needs; more added per stage):
  ```
  addons/NML_Visuals/
    NML_Visuals.gproj
    Scripts/Game/NML/Visuals/      settings module, manager, preset data
    Scripts/Game/NML/Modded/       at most one modded class (entry point), NML_-prefixed members only
    Materials/NML/Visuals/         own copies of vanilla Colors / FilmGrain / ChromAber materials
    UI/Textures/NML/Visuals/       vignette (later: grime) textures
    UI/Layouts/NML/Visuals/        overlay layout (later: settings dialog)
  ```
- Never overrides a vanilla resource (in particular never `ScreenEffects_ColorPP.emat`), never modifies Warfare-Colormod.

### Effect path

- Grade, grain, chromatic aberration (later LUT and softness): **own materials in own camera post-process slots**, all after vanilla's 5–8:

  | Priority | Type | Use |
  |---|---|---|
  | 10 | Colors | NML grade (prototype) |
  | 11 | ColorGrading | LUT (later, if it proves LUT-based) |
  | 13 | ChromAber | edge fringing (later) |
  | 15 | GaussFilter | compression softness (later) |
  | 17 | FilmGrain | grain (prototype) |

  9, 12, 14, 16, 18, 19 stay free. Slots are constants in one place so they can move if testing finds engine conflicts.
- Vignette and (later) lens grime: full-screen HUD overlay `ImageWidget`.
- OFF clears every NML slot and hides the overlay → exactly the underlying baseline (Myrove + Colormod, or vanilla elsewhere).

### Relation to Warfare-Colormod: stack and compensate

Stack in a later slot (our Colors at 10 grades Colormod's output) and tune the NML grade *against* Colormod: give back some saturation, cancel most of the yellow cast, undo the black crush with a small lift. Values below are relative to that baseline; final numbers are set by eye in Workbench with Colormod loaded.

### Presets

| | OFF | AUTHENTIC | HEAVY | EXTREME |
|---|---|---|---|---|
| Saturation (relative) | 1.00 | 1.25 | 1.15 | 1.05 |
| Warmth | — | cancel most of the yellow → neutral-cool | neutral, slightly cool | cool |
| Shadow lift | 0 | +0.015 | +0.025 | +0.035 |
| Grain | off | low | medium | high |
| Vignette | off | very light | light | medium |
| Chromatic aberration | off | barely visible | low | medium |
| Haze / grime / softness | off | off | low | medium |

**Fairness rules (all presets):** never raise exposure; shadow lift must not make dark areas easier to see at night (fade it to zero at night, or drop it if testing shows any gain); grain, vignette, blur only ever reduce visibility.

### Player settings

- `NML_VisualsSettings : ModuleGameSettings`: master on/off, preset, overall intensity, and toggles for grain, vignette + grime, chromatic aberration + softness. The grade has no separate toggle — it *is* the preset.
- Applied live via `OnUserSettingsChangedInvoker()`; persisted in the player profile.
- UI: an own small "NML Visuals" dialog (keybind and/or pause-menu button), **not** a vanilla settings tab, to avoid overriding `SettingsSuperMenu.layout`. The prototype has no dialog (keybind/debug toggle only).

### Limitations accepted

- No custom shaders: no true barrel/fisheye distortion, no sharpening, no real compression artefacts, no scene-reactive highlight clipping or auto-exposure pumping (eye adaptation lives in vanilla `HDR.emat`; overriding it is out of scope).
- Camera roll / head movement is gameplay animation — out of scope.
- Grades add to vanilla effects (bleeding desaturation at 5, blur at 6–7, DOF at 8) — intended.

## Stages (each its own approval gate)

1. **This ADR** — accept or amend.
2. **Prototype** — addon skeleton, settings module (master toggle, OFF/AUTHENTIC), grade at 10, grain at 17, vignette overlay. Verify in Workbench on Myrove with Colormod: first person, third person, ADS/scope, bleeding, night; toggle persists across restart.
3. **Full presets** — HEAVY/EXTREME, chromatic aberration, softness, grime, intensity slider, settings dialog.
4. **Release prep** — docs, validation, Workshop publication (lead approval required).

## Open items (resolved in the prototype stage unless noted)

1. **Entry point**: which single vanilla hook runs the manager on clients (candidates: the game class `ArmaReforgerScripted`, or `SCR_PlayerController`). Vanilla scripts could not be read through the MCP (`game_read` fails on compressed pak scripts; the Workbench Script Editor bridge returned blank lines) — read them in Workbench before choosing.
2. **Material parameters** of FilmGrain, ChromAber, ColorGrading (`.emat` is binary to the MCP): read in Workbench's Material Editor from `UI/Materials/Editor/ColdWar/*.emat`.
3. Whether `SCR_DesaturationEffect` overwrites Colormod's `Saturation` at runtime (visible in the bleeding test).
4. Whether engine effects (HDR, AA, scope PiP, night vision, thermal) occupy slots 9–19; if AA runs after slot 17 it will smear grain. No API lists slots — runtime test.
5. Day/night signal for the night rule.
6. Camera used by scope picture-in-picture.
7. *(Resolved — see below)* Settings UI.
8. *(Resolved — see below)* Copies of vanilla materials.

## Decisions resolved by the lead (2026-10-14)

- **Settings UI (item 7):** an own "NML Visuals" settings dialog. No vanilla Settings-menu tab.
- **Vanilla material copies (item 8):** approved as starting points, on these conditions:
  - copied materials live only inside NML_Visuals;
  - vanilla originals are never edited or overridden;
  - Warfare-Colormod is never modified;
  - all tuning happens on our copies.

## Implementation status — prototype (2026-10-14)

Built: OFF/AUTHENTIC, grade (Colors, slot 10), grain (FilmGrain, slot 17), vignette overlay, settings module, diag-menu controls. Verified on a local diag client (DEV Everon world, game 1.8.0.13) with and without Warfare-Colormod, using `-nmlVisualsCycle` and window captures (`docs/workflows.md#nml_visuals-adr-0007`).

| Check | Result |
|---|---|
| Scripts compile, manager starts once per client | `[NML] Visuals: started (Colormod 0/1, enabled 1, preset AUTHENTIC)`; no NML script or material errors |
| Grade | same frame on vs off: saturation 0.171 → 0.133 (×0.78), red–blue balance 26.4 → 19.0 (cooler) |
| Grain | flat-sky pixel noise 0.03 → 0.74 (subtle, as intended for AUTHENTIC) |
| Vignette | corner luma 112.5 → 83.3, centre unchanged |
| OFF restores baseline | slots cleared, overlay hidden; `cleared on camera 0` |
| `mod_validate`, `tools/validate.mjs` | pass (one warning: enum-only file has no class) |

Findings that change the decision details:

1. **Warfare-Colormod is inert on game 1.8.0.13.** Its `ScreenEffects_ColorPP.emat` fails to parse (`MATERIAL (E): Unknown keyword/data 'Offset'`), and the engine stops at the first unknown key, so none of its values (saturation 0.515, warm colorization) apply. "Stack and compensate" therefore compensates for nothing today: both NML grade materials hold the same values, and `NML_Visuals_Grade_Colormod.emat` is retuned only if Colormod is fixed.
2. **`Offset` is not a `ColorsEffect` parameter**, so the prototype has **no shadow lift** (also the safe choice for the night rule). Confirmed parameters: `Saturation`, `ColorizationColor`.
3. **Entry point:** `ArmaReforgerScripted.OnUpdate` did not fire in a test; the manager starts from `OnWorldPostProcess` and polls every 100 ms on the game call queue.
4. **Overlay z-order:** a negative z-order puts the overlay under the game view (invisible); the default z-order draws it over the HUD, so it is hidden while any menu is open.
5. Grain inherits vanilla `FilmGrainColdWar.emat` unchanged (its parameter names are still unread); tuning happens on our copy in the full-preset stage.

Code review (8 findings) fixed before the PR: diag-menu sync compiled only in diag builds (release builds would otherwise save everything OFF), settings changed elsewhere now update the diag menu instead of being overwritten, slots tracked per camera, overlay rebuilt per world, re-apply on a new controlled entity (respawn), no apply while the camera id is -1.

Not verified in this stage: persistence across restart (needs manual input in the diag menu), pause-menu/map hiding, ADS/scope PiP, Game Master camera, night, bleeding.

## Release v1.0.0 (2026-10-15)

What shipped, and how it differs from the plan above:

- **Presets:** OFF, LOW, AUTHENTIC, HEAVY, EXTREME. One colour identity (the EXTREME grade, chosen by the lead as the anchor); AUTHENTIC uses a lighter version of it, LOW a lighter one still. HEAVY and EXTREME share the grade and differ in grime, vignette and fringing, never by blur.
- **Stack (own materials):** 10 ColorGrading (LUT: tone curve plus colour, generated by `tools/nml-visuals-lut.mjs`), 11 Colors (live saturation trim), 13 ChromAber. Vignette and two lens-grime layers are HUD-root overlays under the HUD.
- **Dropped after testing on game 1.8.0.13:** film grain (the vanilla effect weights grain to bright tones and brightens near-black pixels, which broke the deeper-blacks and night-fairness rules; shadow-weighted variants lifted blacks; the lead found it looked like flat grain), uniform blur (GaussFilter's mask gave no edge-only softness), RotBlur (blacks out the frame), Colors contrast/brightness (act on linear values and crush blacks; tone lives in the LUT).
- **Settings UI:** a small NML VISUALS panel attached to the vanilla pause menu (`NML_VisualsPanel`, own layout, no vanilla layout overridden), per the lead's decision 1. Controls: Look (preset cycle), Vignette and grime, Lens fringing. Persisted in `NML_VisualsSettings` (`m_bEnabled`, `m_iPreset`, `m_bVignette`, `m_bLens`).
- **Deeper blacks:** every LUT toe compresses downward (true black 0; deep shadows a little under the unmodified image); mids and highlights unchanged. The curve points sit on the engine's 16-step LUT lattice (0.286 and 0.401 in display terms) so the toe is not interpolated away.
- **Night fairness:** the shared LowLight LUT only darkens; grime fades out at night; no grain. Myrove night frame mean luminance: off 2.5, all presets 1.3. Daylight p5 luminance is below the unmodified image in every preset.
- **Vanilla materials:** all tuning on NML_Visuals' own copies; no vanilla resource overridden; Warfare-Colormod untouched.
- **Verified:** all presets load with no NML errors on a diag client on Myrove; the panel renders in the pause menu; `mod_validate`, `tools/validate.mjs` and the tool tests pass; code review found nothing release-blocking (fixes applied: unique layout ids, cached presets, diag-only ordering).
- **Persistence:** the profile file `ReforgerGameSettings.conf` holds an `NML_VisualsSettings` block (`m_bEnabled`, `m_iPreset`, `m_bVignette`, `m_bLens`; no grain key), and new client launches with no overrides start on the saved preset (`started (…, preset EXTREME)`).
- **Not verified by Claude:** a mouse click on the panel buttons (synthetic mouse input does not reach the game cursor while a person is using the same machine; the lead's own click is the check); ADS/scope PiP, smoke, explosions, bleeding, vehicles and spectator cameras are left to FRIT's test matrix ([docs/visuals/FRIT-HANDOFF.md](../visuals/FRIT-HANDOFF.md)).
- **Release docs:** [docs/visuals/](../visuals/README.md).

## Consequences

- Zero coupling to NML gameplay and server; can be published and used independently.
- One more addon to maintain; CLAUDE.md and `docs/architecture.md` addon lists gain `NML_Visuals` in the prototype stage.
- Look depends on Colormod's values; a Colormod update can shift the result — re-check its `ScreenEffects_ColorPP.emat` override on each Myrove dependency bump (ADR 0006 process).
