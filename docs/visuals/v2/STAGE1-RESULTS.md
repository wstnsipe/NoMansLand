# NML_Visuals v2 — Stage 1 results (measurement and root-cause validation)

- **Plan:** [STAGE1-PLAN.md](STAGE1-PLAN.md) (approved 2026-11-09). **Stage 0:** [STAGE0-AUDIT.md](STAGE0-AUDIT.md).
- **Status:** measurement build implemented and smoke-tested. Timing sessions (E1/E2) are pending a focused, otherwise idle PC; E3-release and E4 are pending **gate A**.
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

## 4. Hypotheses — status

| # | Hypothesis | Status | Evidence |
|---|---|---|---|
| H0 | NML not the cause | open | needs E3/E4 |
| H1 | persistent PP cost | **not seen in a light scene** (run 1: +0.085 ± 0.113 ms); open under load | needs E2/E4 (production stack, GPU-bound) |
| H2 | overlay fill rate | open | needs E1/E2 timing sessions |
| H3 | slot conflicts | **supported (static)**, runtime open | census §1: all three v1 slots shared; GC 13 confirmed for 1.7.2; visual and spike confirmation needs E2/E5 |
| H4 | clear-then-set spikes | open | needs transition drills |
| H5 | transitions/settings re-apply | open | logging in place (`SETTINGS_OTHER` etc.) |
| H6 | stack / MP interaction | open | needs E2→E4 |
| H7 | extra-world `Start()` | **not seen** so far | 1 start per world in the smoke and run 1 (vanilla); recheck with the production stack (render-target/PiP mods) |
| H8 | slot fight | open | needs E2/E5 |
| H9 | VRAM pressure | open | needs E2/E4 long sessions |

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
