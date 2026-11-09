# NML_Visuals v2 — Stage 1 plan: measurement and root-cause validation

## Context

Stage 0 (PR #15, merged) found no benchmark data for FRIT's report: "rendering performance degradation while NML_Visuals was active on the populated live server with the full production stack; solo testing did not reproduce it".

Stage 1 builds the **smallest diagnostic version of v1** that can confirm **or reject** each Stage 0 hypothesis, and runs it from solo up to populated multiplayer.
- **No v2 features:** no reactive channels, new looks, compat addons or suspend rules.
- **With no launch parameters the build behaves exactly like v1.0.0** (only the log line gains fields). Every change is opt-in through release-safe launch parameters.

## New evidence since Stage 0: static PP slot census

Third-party script contents were read read-only and in memory from the local cache, under the lead's permission. All versions are local cache versions, **UNVERIFIED** against production.
- The API has **no slot getter**, so a runtime census of other mods' slots is impossible.
- The census covers only *script* calls. Effects defined in camera or world configs are not covered; see step 0 below.

Main-camera priorities written by scripts:

| Slot | Writers |
|---|---|
| 5 | vanilla Colors (desaturation); Smokeable Smokes Colors |
| 6 | vanilla RadialBlur; Smokeable RadialBlur |
| 7 | vanilla GaussFilter; ACE Medical pain; **NV-System NVG HDR** (prefab default `m_iLayerId 7`) |
| 8 | vanilla DOF |
| 9 | vanilla regeneration ChromAber (per a Smokeable source comment); Smokeable ChromAber |
| **10** | **NML LUT**; Smokeable RotBlur; CIEThermal optical-NV HDR (PVS-27 uses 10 only on its PiP camera) |
| **11** | **NML Colors trim**; Smokeable DynamicBlur; CIEThermal ThermalImaging; Phaseline thermal; CIE Tracked Core thermal |
| 12 | GC RadialBlur |
| **13** | **NML ChromAber fringe**; **GC Colors** (confirmed in local GC 1.7.2); Phaseline NVG HDR |
| 14 | GC ChromAber; Phaseline NVG Colors |
| 15 | Phaseline grain |
| 16 | Phaseline blur; PVS-27 world HDR |
| 17 | — (no script writer found) |
| 18 | EESuppression RadialBlur; Phaseline DOF |
| 19 | CIE Tracked Core Colors |

Notes:
- S&D Drone Systems uses its own drone camera.
- Realistic Combat Drones writes its priorities inline; they are not resolved yet (UNVERIFIED).
- **All three v1 slots are shared with production mods.** Several mods set *and clear* slots during play, so v1's clear-then-set and their writes can delete each other's effects.

## Hypotheses, variables and decision rules

**Statistic:** the ON−OFF difference in mean frame time over paired, interleaved phases. **SE** is its standard error across pairs. The **noise floor** comes from an OFF/OFF cycle run first in each environment.

| # | Hypothesis | Variable | Confirm if | Reject if |
|---|---|---|---|---|
| H0 | The drop is not caused by NML | V0 vs V5 in E4/E3 | — | — (H0 holds if \|V5−V0\| < 2×SE in E3 **and** E4) |
| H1 | Persistent PP cost | V1 LUT, V2 LUT+trim, V3 fringe | delta ≥ 2×SE **and** ≥ 0.3 ms (solo) / ≥ 2 % (MP), and it scales with output resolution | delta < 2×SE in E2 and E3 |
| H2 | Overlay fill-rate | V4a/b/c, hide-zero | same rule; hide-zero removes the delta at opacity 0 | delta < 2×SE in E2 and E3 |
| H3 | Slot conflicts | slot map `v1` vs `alt` (fringe → 17); with/without GC; E5 thermal and Smokeable drills | conflicts are seen (GC grade or our fringe missing in screenshots; thermal/Smokeable effects lost) **or** spikes that disappear with `alt` | no visual loss and no spike difference |
| H4 | Clear-then-set churn spikes | `legacy` vs `changed` | after-Apply spike rate (> 33 ms within 500 ms of a non-CYCLE Apply) ≥ 3× the background rate, and gone in `changed` | ratio < 1.5× |
| H5 | Transitions/settings trigger applies | reason counters; settings filter | apply counts per drill > 1 per transition, or `SETTINGS_OTHER` > 0 | ≤ 1 per transition and no `SETTINGS_OTHER` |
| H6 | Interaction with stack / MP load | E1→E4, mod bisect | the H1/H2/H4 delta in E3/E4 is ≥ 2× the E1 delta and ≥ 2×SE; bisect removes it | E3/E4 delta within 2×SE of E1 |
| H7 | `OnWorldPostProcess` fires for extra worlds (preview / render-target) and re-applies | `Start()` count per world, `WORLD` applies | more than 1 per real world load | exactly 1 |
| H8 | Other mods keep re-writing our slots (slot fight) | re-assert count per slot, `changed` + re-assert triggers | our slot gets re-asserted repeatedly during gameplay, and spikes correlate | no re-asserts outside transitions |
| H9 | VRAM pressure (184 mods + our textures) | V-1 vs V5; texture memory if exposed | the delta grows over session time or with view distance | flat |

**Covariates** are recorded per run rather than tested: GPU, driver, graphics API, upscaler, resolution, and video preset.

## Step 0: preconditions (before any measurement)

1. **`game_read` the vanilla camera/world PP configs.** Confirm slot 17 is not defined in config; if it is, choose another free slot or drop `alt`.
2. **`api_search` / wiki:** confirm that `CallLater(fn, 0, true)` runs once per frame and keeps running in menus.
3. **Cache guard:** snapshot the Arma Workshop cache (name, size and modified time of every folder and `*_manifest.json`) before and after **every** local session. Every server and client process runs with `-addonDownloadDir <repo>/.local/...`. If anything in the cache changed, stop and report.

## Implementation (addons/NML_Visuals only)

### 1. Launch parameters (release builds; read once in `Start`; camelCase and lowercase both accepted, like v1)

| Param | Values | Default (= v1) |
|---|---|---|
| `-nmlvLayers` | comma list of `lut,trim,fringe,vignette,grime,grimeheavy` or `none` | user settings (wins over the diag `-nmlVisualsOnly` if both are set) |
| `-nmlvApply` | `legacy` / `changed` (set only changed slots, never clear before set, clear only on disable, re-assert on camera/controlled change) | `legacy` |
| `-nmlvSlots` | `v1` (10/11/13) / `alt` (fringe → 17; LUT and trim stay 10/11 so order and look are unchanged) | `v1` |
| `-nmlvSettingsFilter` | `0`/`1`: ignore saves that don't change our 4 values | `0` |
| `-nmlvHideZero` | `0`/`1`: `SetVisible(false)` below 0.005 opacity; push opacity only on change | `0` |
| `-nmlvPreset` | session-only preset | saved setting |
| `-nmlvCycle` | `<sec>` (OFF/ON) or `<sec>:<layersA>/<layersB>`, ABBA order; **settings are never saved while cycling** | off |
| `-nmlvProbe` | `<sec>` summary interval | off |

Invalid values log one warning and use the default.

### 2. Apply bookkeeping (always on; trivial cost)

- `NML_EVisualsApplyReason`: `WORLD, CAMERA, CONTROLLED, LOWLIGHT, SETTINGS_OURS, SETTINGS_OTHER, USER, CYCLE`.
- Counters per reason.
- `Start()` count per world (H7).
- Re-asserts per slot (H8).
- The existing one-line log per `Apply` gains the reason, slot map, layers and mode.

### 3. `NML_VisualsProbe` (only with `-nmlvProbe`; **on in both arms** so its cost cancels)

- Per-frame tick: `dt` from `System.GetTickCount()`.
- **1 ms bins 0–127 plus overflow** in a fixed `int[129]`; frames, max `dt`. No allocations.
- **Mean frame time** = elapsed ÷ frames over the phase; that is exact. `GetFPS` is not used.
- **Discard the first 5 s** after every phase switch (material and texture first-load).
- **Spike attribution:** spikes > 33 ms within 500 ms after a non-CYCLE Apply, counted separately from background spikes; CYCLE applies are reported on their own.
- **Summary line per interval:** phase label, frames, mean ms, p50/p95/p99 from the histogram, spikes > 33/50/100 ms, applies by reason, after-Apply spikes, camera and controlled-entity changes, settings events (ours/other), `Start()` count, slot re-asserts, layers/slots/mode, player count.
- **View type** (1P/3P, ADS, PiP, vehicle, non-character) is sampled **only at summary time**.
- **Probe cost bound, stated analytically:** one call per frame with integer arithmetic, and one log line per interval. Native `LogFPS` cannot resolve < 0.05 ms, so this is not "measured".

### 4. Tooling

- `tools/nml-visuals/probe-summary.mjs`: parses logs into paired ON−OFF deltas with SE, ABBA-aware, and applies the decision rules.
- `tools/tests/probe-summary.test.mjs` with synthetic fixtures.
- A cache-guard snapshot/compare script: `tools/nml-visuals/cache-guard.mjs`, read-only on the cache.

### 5. Docs

- `docs/visuals/v2/STAGE1-PLAN.md` (this plan).
- `docs/visuals/v2/STAGE1-RESULTS.md` (census addendum, results, conclusions).

**Not in Stage 1:** v2 state model, hub or renderer; new visuals; compat addons; suspend rules; removing v1 features; any change to release defaults.

Critical files:
- `addons/NML_Visuals/Scripts/Game/NML/Visuals/NML_VisualsManager.c`
- new `NML_VisualsProbe.c` and `NML_EVisualsApplyReason.c` beside it
- `Scripts/Game/NML/Modded/ArmaReforgerScripted.c` (count `Start()` per world only)

Reused v1 paths: `UseLayer`, `ClearSlots`, `PushParam`, `UpdateOverlay`, `SampleWeather`.

## Variants and environments

**Variants:**

| ID | Configuration |
|---|---|
| V-1 | NML_Visuals not loaded (separate sessions; FRIT's actual comparison) |
| V0 | OFF |
| V1 | LUT |
| V2 | LUT + trim |
| V3 | fringe |
| V4a/b/c | vignette / grime / grimeHeavy |
| V5 | full v1 `legacy` |
| V6 | full `changed` + settings filter + hide-zero |
| V7 | V5 + `alt` (fringe 17) |

Each is paired with OFF or its partner via `-nmlvCycle`, ABBA, 60 s phases, ≥ 8 pairs, same spot and view. An OFF/OFF noise run comes first.

**Environments** (build type matters):

| Env | Setup | Build | Weight |
|---|---|---|---|
| E1 | solo, vanilla terrain, NML only | **release** client loading the locally built package via `-addonsDir build/…` (no publish needed) | **valid** |
| E2 | solo, production stack (Bakhmut / Ovruch), with and without GC | release client, production mods read-only from the cache, our package from `build/` | **valid** |
| E3-diag | local server + 2–3 clients, production stack | diag executables (existing `server-dev.ps1` / `client-dev.ps1`) | **directional only:** diag compiles extra code. One measured client at FRIT-like resolution and settings; the others minimised on low settings. One PC shares the GPU, which is a stated distortion. |
| E3-release | small MP, release, server config mode | needs the test build downloadable from the Workshop, plus a server config | **valid; ⚠ approval gate A** |
| E4 | populated, live-like (FRIT), with E5 combat stress | released test item | **valid; ⚠ approval gate A** |

**E5 combat stress** runs inside E2/E3/E4: smoke, grenades and explosions, sustained suppression, bleeding, vehicles, drone flight, scopes (PiP), NVG, CIEThermal and Phaseline thermal, and Smokeable effects (the slot 10/11 clashes).

**Transition drills** (E2/E3), for H4/H5/H8:
- respawn ×10;
- vehicle enter/exit ×10;
- drone fly/exit ×5;
- NVG on/off ×10;
- ADS scope ×20;
- change a video setting ×5.

**GPU-bound check:**
- Vary **output resolution** (window size), not render scale; UI overlays and possibly PP run at output resolution.
- PresentMon would give true GPU-busy time. It is a third-party install, so **approval gate B**.

**Mod bisect:** local only (E2/E3). GC, EESuppression, Smokeable, NV-System, Phaseline NVG, CIEThermal, drones, 2-7 Suppressor Overheating.

## Approval gates (I stop and ask at each)

- **A — before E3-release and E4.** Upload an **unlisted Workshop test item**:
  - its own GUID and name (e.g. "NML_Visuals Probe"), never published over 81E04280BF506AF2;
  - a load-time error log if v1 is also loaded, because duplicate class names would fail to compile, so the two must never be listed together;
  - a `.local` (not `server/`) server config for E3-release;
  - E4 runs on a **separate FRIT test session or instance**, not the live public server, so ordinary players don't get the pulled v1 behaviour again.
- **B — any external profiler install** (PresentMon).
- **C — any change to released defaults or the live server.** None planned.

## Verification

- `mod_validate` NML_Visuals; Workbench compile with no script errors; `mod_build` to `build/`.
- `node tools/validate.mjs`; `node --test` for probe-summary and cache-guard.
- **No-params parity:** a v1 log diff shows only the added fields; no probe object; settings untouched.
- Cache guard passes after every local session.
- ECC code review plus a security-style pass on launch-parameter parsing; fix findings.
- Branch `feat/visuals-v2-stage1`; local commits; push and PR at the end of the stage. **Merge needs approval.**

## Outcome

`STAGE1-RESULTS.md` records each hypothesis as confirmed, rejected or inconclusive, with numbers. That feeds Stage 2 (ADR 0008): which v1 components to keep, change or drop, plus the slot, apply and overlay policies.

If E1–E3 reproduce nothing, the gate-A request includes the exact probe command lines and a FRIT script.

## Estimates

- Step 0 and implementation with tests: about 1–1.5 days.
- E1/E2/E3-diag runs and analysis: about 1 day.
- E3-release and E4 depend on gate A and FRIT scheduling.
