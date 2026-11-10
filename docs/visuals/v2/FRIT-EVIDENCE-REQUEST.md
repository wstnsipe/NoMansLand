# NML_Visuals v1: evidence request for FRIT

We're trying to find out why NML_Visuals v1 hurt rendering on the populated server. Local tests with the full 184-mod stack show v1 costing at most about 0.2 ms per frame, with no stutter, so we need data from the sessions where you saw the problem. Send whatever you still have. Partial answers help, and "don't know" is a fine answer.

## 1. Logs (most useful)

Client `console.log` files from any session where the slowdown happened while NML_Visuals was installed (v1.0.0, from 2026-10-15). On Windows they're in:

```
Documents\My Games\ArmaReforger\profile\logs\logs_<date>_<time>\console.log
```

(or the same path under `OneDrive\Documents\...`). Zip the whole `logs_<date>_<time>` folders for the affected dates. One normal session from the same period, without the problem, would also help as a comparison. Server logs from those times are welcome too.

## 2. Questions

| # | Question | Your answer |
|---|---|---|
| 1 | Roughly how many players were on when it happened? | |
| 2 | Did it start right away, only during combat, or after playing for a while? | |
| 3 | Did switching NML_Visuals to **OFF** (Esc → NML VISUALS → Look) fix it, improve it, or change nothing? Did anyone try? | |
| 4 | Which preset was used (LOW / AUTHENTIC / HEAVY / EXTREME)? Were vignette/grime and lens fringing on? | |
| 5 | GPU model | |
| 6 | Resolution | |
| 7 | Video preset (Low / Medium / High / Ultra / custom) | |
| 8 | Upscaler (DLSS / FSR / none) and its quality setting, if known | |
| 9 | What did it look like: steady lower FPS, short stutters/hitches, broken or flickering image, or a mix? | |
| 10 | Did it seem to line up with any of these: being suppressed, smoke, thermals, NVGs, drones, respawning, getting in/out of vehicles, scopes? | |
| 11 | Did other players report the same thing? Roughly how many, and on which hardware? | |

## How we'll use it

The v1 logs record every time the mod re-applied its effects. We'll count those per session and line them up with camera, world, menu and settings events (`tools/nml-visuals/v1-log-analysis.mjs`). Answers are recorded as **your recollection** and kept separate from what the logs prove. Nothing is shared outside the project.
