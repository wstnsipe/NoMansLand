# NML_Visuals v2 — Stage 1 results (measurement and root-cause validation)

- **Plan:** [STAGE1-PLAN.md](STAGE1-PLAN.md) (approved 2026-11-09). **Stage 0:** [STAGE0-AUDIT.md](STAGE0-AUDIT.md).
- **Status:** measurement build implemented. Local runs 1–4 done (vanilla, then the full production stack); see §3 and the interim conclusion. Gates A and B were declined (2026-11-09).
- **All third-party versions:** local Workshop cache versions, **VERSION UNVERIFIED** against production.

## 1. Static PP slot census (complete)

Third-party and vanilla scripts were read **read-only, in memory**, from the local cache and the game's own paks. Nothing was extracted to disk or modified.
- The API has no slot getter, so this static census is the census.
- Effects defined purely inside the engine (HDR tonemap, AA) are not visible to any of these methods.

Main player camera, priority → writers:

| Slot | Writers |
|---|---|
| 5 | vanilla `SCR_DesaturationEffect` Colors; Smokeable Smokes Colors |
| 6 | vanilla `SCR_StaminaBlurEffect` RadialBlur; Smokeable RadialBlur |
| 7 | vanilla `SCR_DamageBlurEffect` GaussFilter; ACE Medical pain; NV-System NVG HDR (prefab default layer 7); Realistic Combat Drones FilmGrain (drone view) |
| 8 | vanilla `SCR_DepthOfFieldEffect` DOF; Realistic Combat Drones Colors (drone view) |
| 9 | vanilla `SCR_RegenerationScreenEffect` ChromAber; Smokeable ChromAber; Realistic Combat Drones ChromAber (drone view) |
| **10** | **NML LUT**; Smokeable RotBlur; CIEThermal optical-NV HDR |
| **11** | **NML Colors trim**; Smokeable DynamicBlur; CIEThermal / Phaseline / CIE Tracked Core ThermalImaging |
| 12 | GC Suppression RadialBlur |
| **13** | **NML ChromAber fringe**; **GC Suppression Colors** (1.7.2); Phaseline NVG HDR |
| 14 | GC ChromAber; Phaseline NVG Colors |
| 15 | Phaseline grain |
| 16 | Phaseline blur; PVS-27 world HDR |
| 17 | — no script or config writer found (used for `-nmlvSlots alt`) |
| 18 | EESuppression RadialBlur; Phaseline DOF |
| 19 | CIE Tracked Core Colors |

Other cameras:
- S&D Drone Systems uses its own drone camera.
- PVS-27 uses slot 10 on its PiP camera.

Realistic Combat Drones **clears 7/8/9 on exit from drone view**, which also removes vanilla and ACE effects in those slots; that's outside NML.

**Reading:** every v1 slot (10, 11, 13) is shared with at least one production mod that sets and clears it during play.

## 2. What the measurement build provides

All switches are launch parameters that work in release builds (see plan §1). With **no** parameter the build behaves like v1.0.0. Verified:
- no config line;
- no probe;
- the same `applied …` line, gaining only `reason/layers/mode/fringeSlot/sets/clears/reasserts`;
- no script errors.

Probe output is one line per interval:
```
[NML] Probe: phase=A:off ms=9676 frames=1280 mean100=755 p50=7 p95=10 p99=12 max=16 s33=0 s50=0 s100=0 aSpk=0 aMax=0
  applies=WORLD,CAMERA,CONTROLLED,LOWLIGHT,SETTINGS_OURS,SETTINGS_OTHER,USER,CYCLE counts sets= clears= reasserts=
  cam= ctl= setOurs= setOther= starts= layers= mode= fringeSlot= filter= hideZero= preset= night= view= players=
```

Analysis: `node tools/nml-visuals/probe-summary.mjs <console.log>…` gives:
- the paired B−A delta and its SE across ABBA pairs;
- the verdict: |delta| ≥ max(2, t₀.₉₇₅,n−1)×SE, which is the plan's 2×SE made stricter when there are few pairs;
- spike rates, apply counts and after-apply spikes.

Timer resolution: `System.GetTickCount()` gave p50/p95/p99 of 7/10/12 ms in the smoke run, which are not multiples of ~15 ms, so 1 ms resolution holds on this machine.

Cache guard: `node tools/nml-visuals/cache-guard.mjs snapshot|compare`, run before and after every session that loads mods from the real cache.

## 3. Evidence so far

| Check | Result |
|---|---|
| Compile (diag client, NML_Dev_Everon) | OK after one fix (`set` is a reserved type name in Enforce) |
| No-parameter parity | OK (see §2) |
| Probe + cycle smoke (`-nmlvProbe 10 -nmlvCycle 20`) | config line, probe lines, ABBA phase labels, apply reasons (`WORLD`, `CYCLE`) all logged as designed |
| Tool tests | `node --test tools/tests/nml-visuals-stage1.test.mjs`: 7/7 pass |
| Smoke finding: **single-player pauses on focus loss** | The pause menu opened when the window lost focus; the game call queue stops, so probe and cycle stop. **Timing sessions need a focused, otherwise idle machine**, or multiplayer (a server + client does not pause). |
| Smoke finding: interval boundary | The first A interval was flushed after the cycle switched state (state field wrong), and the world-load frames were counted. Both fixed: flush before switching, discard the first 5 s. |

### Run 1 — directional, vanilla (2026-11-09)

- **Setup:** local diag dedicated server (`tools/server-dev.ps1 -World Worlds/NML/Dev/NML_Dev_Everon.ent -IncludeVisuals`) and one diag client (`client-dev.ps1 -IncludeVisuals -ExtraArgs "-nmlvProbe 10 -nmlvCycle 45"`).
- **Content:** NML addons only, no production stack. 1 player, first person, AUTHENTIC preset, ~155 fps.
- **Conditions:** the PC was in use during the run, so this is **directional only** (diag binaries, shared GPU).
- **Log:** archived locally at `.local/research/stage1-runs/2026-11-09_E3diag_vanilla_V5vsV0.console.log`.

| Measure | A = OFF | B = full v1 (LUT, trim, fringe, vignette; grime 0 in AUTHENTIC) |
|---|---|---|
| Mean frame time | 6.51 ms | 6.52 ms |
| Frames | 55,354 | 49,850 |
| Spikes > 33 ms per 1k frames | 0.00 | 0.06 (one 151 ms hitch, not after an Apply) |

- **B − A:** +0.085 ms (+1.3 %), SE 0.113 ms over 9 ABBA pairs. **Verdict: no difference** (|delta| < 2.31 SE).
- **Applies:** WORLD 1, CONTROLLED 1, CYCLE 17. After-apply spikes: 0.

**Reading:** with a light scene, a single player and no production mods, v1's full stack (3 PP passes plus overlays) has no measurable frame-time cost and causes no apply hitches. This matches FRIT's "solo is fine". The cost, if real, has to come from the production stack, population or GPU-bound conditions (H1 under load, H3, H6, H8), which runs 2–4 must test.

### Runs 2–4 — full production stack (184 mods), local (2026-11-09)

**Setup**
- All 184 Custom #784 mods load read-only from the lead's local Workshop cache (`-ExtraAddonsDir`), with downloads redirected to `.local/research/dl`.
- **Cache guard: no change after any of the 5 sessions** (snapshots in `.local/research/cache-guard/`).
- Logs: `.local/research/stage1-runs/`.

**Two local limits found**
1. **Local multiplayer with the production stack is impossible.** The diag server kicks the diag client: `RplConnection::ValidationError script protocol checksum does not match`. Some production mod compiles differently on a dedicated server than on a client, so runs 2–4 are single player (`client-dev.ps1 -World`).
2. **The production scenario (`Missions/NML_Ovruch.conf`, world `Worlds/NML_Ovruch.ent`) stops in the role-selection menu** in single player, which needs UI input to leave. Runs 2–4 therefore load the production stack on the NML dev world, which auto-spawns the player in first person. That covers the production mods' scripts, screen effects and camera handlers, but not the Ovruch terrain or combat events.

**Common to every run**
- Diag binary, single player, PC in use: directional only.
- First person, `-nmlvProbe 10 -nmlvCycle 45` (ABBA).

| Run | Resolution | Preset | A = OFF mean | B = full v1 mean | B − A | SE (pairs) | Verdict |
|---|---|---|---|---|---|---|---|
| 2 | 1280×720 | AUTHENTIC | 9.52 ms | 9.73 ms | **+0.195 ms (+2.1 %)** | 0.089 (7) | no difference (t 2.19 < 2.45), but every pair is positive |
| 3 | 1920×1080 | AUTHENTIC | 11.04 ms | 11.02 ms | −0.098 ms (−0.9 %) | 0.246 (8) | no difference |
| 4 | 1920×1080 | EXTREME (both grime layers) | 10.93 ms | 11.11 ms | **+0.163 ms (+1.5 %)** | 0.067 (8) | **differs**, but below the 0.3 ms / 2 % materiality bar |

Notes:
- **Spikes:** runs 2–4 had 46 cycle switches, each a full clear-and-set of all slots, plus 3 WORLD applies and 6 SETTINGS_OTHER applies. **There were 0 spikes after an Apply in any run**, and background spikes stayed at ≤ 0.4 per 1,000 frames.
- **Load:** the production stack raises the base frame time from 6.5 ms (vanilla, run 1) to 9.5 ms at 720p.
- **CPU-bound:** moving from 720p to 1080p (2.25× the pixels) only went from 9.5 to 11.0 ms. This machine is CPU-bound, so per-pixel costs are mostly hidden here.

## 4. Hypotheses — status

| # | Hypothesis | Status | Evidence |
|---|---|---|---|
| H0 | NML not the cause | **supported locally** | v1 costs ≤ 0.2 ms (≤ 2 %) with all 184 mods; no hitches. Not provable without populated MP (gate A or FRIT logs) |
| H1 | persistent PP cost | **measurable, immaterial here** | runs 1–3 show no difference; it doesn't scale 720p→1080p on this CPU-bound PC. Open only for GPU-bound clients (high resolution, weaker GPUs) |
| H2 | overlay fill rate | **measurable, immaterial here** | run 4 (EXTREME, both grime layers): +0.163 ± 0.067 ms (+1.5 %) |
| H3 | slot conflicts | **confirmed statically**; runtime not exercised | census §1. Combat events (suppression, thermal, Smokeable) need input or MP, which local runs can't provide |
| H4 | clear-then-set spikes | **rejected (steady state)** | 46 full clear-and-set cycles plus 3 world applies with the production stack: 0 after-Apply spikes |
| H5 | transitions/settings re-apply | partly seen | `SETTINGS_OTHER` 2 per session (startup saves by other mods). Respawn, vehicle, drone and NVG drills not run (need input) |
| H6 | stack / MP interaction | **weak** | the stack raises base load (6.5 → 9.5 ms), but v1's own cost stays ≤ 0.2 ms. MP not testable locally (checksum kick) |
| H7 | extra-world `Start()` | **rejected** | 1 start per world in all 6 sessions, including the production stack |
| H8 | slot fight | open | needs E2/E5 |
| H9 | VRAM pressure | open | needs E2/E4 long sessions |

### Interim conclusion (2026-11-09)

**Under local conditions, NML_Visuals v1 is not a material rendering cost, even with the full production stack:**
- steady-state cost is ≤ ~0.2 ms (≤ 2 %), and only measurable for EXTREME;
- re-applying (clear-then-set) causes no hitches;
- `Start()` is not duplicated.

**What remains unexplained can't be reproduced locally:**
- **Slot conflicts during combat (H3/H8).** GC Suppression, Smokeable Smokes and thermal/NVG mods write slots 10/11/13 during play. This mainly predicts a broken or doubled look, not lower FPS.
- **GPU-bound clients (H1/H2 at high resolution or on weaker GPUs).** Per-pixel cost could reach a few percent there; it is hidden on this CPU-bound PC.
- **Populated multiplayer (H6/H0).** Local multiplayer with this stack is blocked by the script checksum mismatch.

The most likely reading is that the reported degradation was **not mainly caused by v1's own rendering or re-apply cost**. If v1 contributed, it was through combat-time slot fights or GPU-bound clients.

## 5. Measurement protocol (to run)

Each run alternates two variants in the same spot and view. Use `-nmlvCycle 60` with ABBA order, at least 8 pairs, and `-nmlvProbe 10` in both arms. Start with an OFF/OFF noise run (`-nmlvCycle 60:none/none`).

| Variant | Extra parameters |
|---|---|
| V5 vs V0 | `-nmlvCycle 60` (full v1, OFF/ON) |
| V1 / V2 / V3 vs V0 | `-nmlvCycle 60:none/lut`, `…:none/lut,trim`, `…:none/fringe` |
| V4a/b/c vs V0 | `…:none/vignette`, `…:none/grime`, `…:none/grimeheavy` (with `-nmlvPreset EXTREME` so grime is non-zero) |
| V6 | `-nmlvApply changed -nmlvSettingsFilter 1 -nmlvHideZero 1` + cycle |
| V7 | `-nmlvSlots alt` + cycle |

Notes:
- A GPU-bound check repeats key variants at a smaller **output resolution** (window size).
- Transition drills (respawn, vehicle, drone, NVG, ADS, video setting) run with the probe on, using `legacy` vs `changed`.
