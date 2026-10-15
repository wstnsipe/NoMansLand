# NML_Visuals

Optional client-side visual mod for Arma Reforger: a helmet-cam / bodycam combat-footage look. Standalone, base-game dependency only. It changes only what the local player sees. It has no gameplay, server, persistence or replication code. Design record: [ADR 0007](../adr/0007-nml-visuals.md).

- **Version:** 1.0.0
- **Addon:** `addons/NML_Visuals` (GUID `81E04280BF506AF2`)
- **Dependencies:** base game only (no NML_Core, NML_Content, Myrove or third-party mods)
- **Related:** [CHANGELOG](CHANGELOG.md), [Workshop text](WORKSHOP.md), [FRIT handoff](FRIT-HANDOFF.md)

## What it does

Every preset except OFF applies the same colour identity: flat, bright mids, deep blacks, early hard highlight clipping, pale cyan sky, olive foliage, restrained saturation. No orange/teal, no film grain, no blur. Strength grows through camera imperfection, not through a different grade.

| Preset | Grade | Fringing | Vignette | Lens grime | Weather/time reaction |
|---|---|---|---|---|---|
| OFF | none (vanilla image) | none | none | none | none |
| LOW | light: brighter mids, slightly pale sky | hint | light | none | 0.8x |
| AUTHENTIC | the family grade, lighter, softer highlight roll-off | barely visible | light | none | 1.0x |
| HEAVY | full family grade, harder clipping | subtle | medium | smudges at the edges | 1.3x |
| EXTREME | same grade as HEAVY | visible but restrained | strong | dense, irregular dirt and streaks at the edges; centre stays clear | 1.6x |

Time and weather: overcast, rain and dusk mute the colour a little more (more in the stronger presets); rain also makes the grime a little stronger. Night uses a separate grade that only darkens.

**Fairness:** the look never raises exposure. Blacks are deeper than the unmodified image, the night grade is darker than the unmodified image, and grime fades out at night, so no preset gives a visibility advantage. Measured on a Myrove night frame (mean luminance): off 2.5, every preset 1.3.

## Install and use

**Players:** subscribe to the mod in the Workshop (or have the server's mod list include it, see below). In game press **Esc**. The **NML VISUALS** panel is at the bottom right of the pause menu:

- **Look** cycles OFF, LOW, AUTHENTIC, HEAVY, EXTREME.
- **Vignette and grime** turns the two overlays on or off.
- **Lens fringing** turns chromatic fringing on or off.

Settings are saved in the player's own profile. Fresh installs start on AUTHENTIC.

**Servers:** Reforger clients load exactly the server's mod list, so a server that wants the look available must list the mod. Add the Workshop ID and name to the server's `game.mods` (order does not matter, the mod depends on nothing but the base game). Every connecting client then downloads it and chooses a preset locally. A dedicated server loads the mod but runs nothing from it.

**Disabling:** choose OFF in the panel, or remove the mod from the mod list.

## Known limitations

- No true fisheye or barrel-distortion shader (the engine has no custom-shader hook).
- No video-codec compression simulation (blocking, banding, smeared foliage).
- No motion blur: the engine's RotBlur effect blacks out the frame on game 1.8.0.13, and the server already has bodycam head movement.
- No film or sensor grain. The engine's grain effect brightens near-black pixels, which conflicts with the deeper-blacks and night-fairness rules, and it reads as flat grain.
- No sharpening halos and no auto-exposure pumping.
- Scope picture-in-picture: the grade was seen applied to a scope view in a test screenshot, but the effect on PiP optics is not systematically verified. Report anything odd.
- Diagnostics hotkeys (F6/F7) and the diag menu exist only in diagnostic game builds, not in retail.
- Warfare-Colormod's colour override fails to parse on game 1.8.0.13 (its own file), so it currently changes nothing. If it is fixed, the NML trim material `NML_Visuals_Grade_Colormod` may need retuning.

## Regenerating the textures

The colour lookup tables and grime overlays are generated, deterministic assets:

```
node tools/nml-visuals-lut.mjs --all        # LUT specs in tools/nml-visuals/luts/*.json -> Textures/NML/Visuals/*_LUT.png
node tools/nml-visuals-textures.mjs         # grime overlays -> UI/Textures/NML/Visuals/*.png
```

Workbench must be running when a PNG changes: it rebuilds the runtime `.edds`. Committed textures are Git LFS and read-only until locked.
