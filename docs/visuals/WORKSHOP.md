# NML_Visuals: Workshop listing and publish steps

## Listing text

**Name:** NML Visuals

**Summary:** Optional helmet-cam combat-footage look for Arma Reforger. Client only, no gameplay changes.

**Description:**

NML Visuals gives the game the look of modern helmet-cam and bodycam combat footage: flat bright mids, deep blacks, hard highlight clipping, pale sky, olive foliage, and a dirty lens. It is purely visual, runs only on your own screen, and changes nothing for the server or other players. It never makes the night easier to see: blacks and night are darker than the unmodified image.

Presets (press Esc, use the NML VISUALS panel at the bottom right of the pause menu):
- OFF: vanilla image.
- LOW: a light touch of the grade.
- AUTHENTIC: the full look, clean lens.
- HEAVY: harder clipping, smudged lens edges.
- EXTREME: the same colours with a dense, dirty lens. The centre of the screen stays clear.

Also in the panel: vignette and grime on or off, chromatic fringing on or off. Your choice is saved per player.

Requirements: base game only. No other mods. Servers that want the look available must add the mod to their mod list so clients download it.

Known limitations: no fisheye distortion, no codec compression, no motion blur, no film grain.

Version 1.0.0.

## Settings to use in the Workbench publish dialog

- Project: `NML_Visuals` (`addons/NML_Visuals/NML_Visuals.gproj`, GUID `81E04280BF506AF2`)
- Dependencies: none beyond the base game (the dialog should list none)
- Version: 1.0.0
- Changelog text: copy the 1.0.0 entry from [CHANGELOG.md](CHANGELOG.md)
- Publish folder: leave the default outside the repo (`Documents\My Games\ArmaReforger\publish\NML_Visuals`)
- Preview image: any of the review images in `.local/visuals-review/` (not committed)
- Visibility: the dialog's options are unconfirmed (see [release.md](../release.md)); choose the most restrictive option that lets FRIT's server and test clients download it, and record what you chose
