# ADR 0007: NML_Visuals — optional client-side combat-footage look

- **Status:** Proposed (2026-10-14). No implementation until the lead accepts this ADR and approves the prototype stage.

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
7. *(Lead)* Settings UI: own dialog (recommended) vs vanilla settings tab.
8. *(Lead)* Copying vanilla materials into an NML addon as starting points — same practice as other content; recorded, no conclusion drawn.

## Consequences

- Zero coupling to NML gameplay and server; can be published and used independently.
- One more addon to maintain; CLAUDE.md and `docs/architecture.md` addon lists gain `NML_Visuals` in the prototype stage.
- Look depends on Colormod's values; a Colormod update can shift the result — re-check its `ScreenEffects_ColorPP.emat` override on each Myrove dependency bump (ADR 0006 process).
