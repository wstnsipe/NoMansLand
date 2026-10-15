# NML_Visuals v1.0.0: FRIT handoff

NML_Visuals is an optional client-side look (helmet-cam style). It depends on the base game only. It never changes gameplay, the server or other players. Overview: [README](README.md).

## 1. Add it to the NML server

1. Workshop item **NML_Visuals**, ID **81E04280BF506AF2**, version **1.0.0**, unlisted (reachable by ID and link, not in search): https://reforger.armaplatform.com/workshop/81E04280BF506AF2-NML_Visuals
2. Add it to the server's mod list (`game.mods`: `{ "modId": "81E04280BF506AF2", "name": "NML_Visuals", "version": "1.0.0" }`). Order does not matter. It has no dependencies.
3. Start the server. The log must show no `NML` script errors. The server loads the mod and does nothing with it (the manager disables itself on dedicated servers).

## 2. Check that clients get it

1. Join with a client that does not have the mod yet. It downloads with the other mods.
2. Press Esc. At the bottom right of the pause menu there is an **NML VISUALS** panel with three buttons: Look, Vignette and grime, Lens fringing. A fresh install starts on AUTHENTIC.
3. Click **Look** to step through OFF, LOW, AUTHENTIC, HEAVY, EXTREME. Close the game, reopen it: the choice must still be set.

## 3. Test matrix

For each preset (OFF, LOW, AUTHENTIC, HEAVY, EXTREME), check:

| Area | Check |
|---|---|
| Weather and time | clear, overcast, rain, dusk, night: the look stays believable; night is never easier to see than OFF |
| Places | interiors, trenches, dense woods: blacks are deep, nothing is unplayable |
| Optics | ADS iron sights and scopes (PiP): picture is correct, no black frame, no artefacts |
| Combat | smoke, explosions, muzzle flash, bleeding/injury effects: no flicker, no broken frames |
| Bodycam | with the existing bodycam / head-movement system: stationary image sharp, no unwanted blur |
| UI | HUD stays clean (the grime sits under it); inventory, map and menus unaffected |
| Panel | all three buttons work; settings survive a restart |
| Cameras | respawn, spectator/Game Master camera, vehicles: the look re-applies, no errors |

## 4. Report back

- Any `NML` or `Visuals` errors in the client or server log (search `[NML] Visuals`).
- Conflicts with other mods or other post-process effects.
- Performance impact, if any.
- Which preset should be the **default** for new players (today: AUTHENTIC).
- Anything that gives a visibility advantage.
