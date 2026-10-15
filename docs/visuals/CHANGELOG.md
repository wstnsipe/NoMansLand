# NML_Visuals changelog

## 1.0.0 (2026-10-15)

First release.

- Five presets: OFF, LOW, AUTHENTIC, HEAVY, EXTREME. One shared colour identity; strength comes from lens grime, vignette and fringing, not from a different grade and not from blur.
- Colour grading through 256x16 LUT textures generated from readable specs (`tools/nml-visuals/luts/`): bright flat mids, deep blacks, hard highlight clip, pale cyan sky, olive foliage. A separate night LUT that only darkens.
- Live saturation trim driven by overcast, rain and dusk; stronger presets react more.
- Chromatic fringing (`ChromAber`), vignette and two lens-grime overlays drawn under the HUD (the HUD stays clean).
- Pause-menu panel (Look, Vignette and grime, Lens fringing). Per-player settings in the player profile via `NML_VisualsSettings`; nothing is replicated.
- Night fairness: grade darker than unmodified at night, grime fades out at night.
- Client only: inert on a dedicated server. Depends on the base game only.
- Deeper blacks: the lower end of every LUT compresses downward; mids and highlights are unchanged.
- Removed during development, not shipped: film grain, uniform blur (GaussFilter), motion blur (RotBlur), edge-mask experiments, a separate HEAVY grade.
