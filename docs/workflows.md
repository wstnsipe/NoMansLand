# Workflows

## Daily loop (DEV)

1. `git switch main && git pull`, then `git switch -c feat/<system>-<desc>`.
2. Start Workbench on the local wrapper: `powershell -ExecutionPolicy Bypass -File tools\wb-dev.ps1`.
   - It copies the Enfusion MCP handler scripts into the wrapper (`.local\NML_Dev\Scripts\WorkbenchGame\EnfusionMCP\`, gitignored), then launches Workbench with `-gproj .local\NML_Dev\NML_Dev.gproj -addonsDir addons` so the NML addons resolve from your checkout ([ADR 0002](adr/0002-local-dev-wrapper.md)).
   - The Enfusion MCP attaches once the Net API (port 5775) is up. Check with `wb_connect`.
3. Edit scripts in your editor (or with Claude); edit prefabs and worlds in Workbench. Reload scripts, test in Play mode (the DEV world is `NML_Scenario_Dev/Worlds/NML/Dev/NML_Dev_Everon.ent`).
4. **Close Workbench** before starting a server, client or Autotest run. They read each addon's `resourceDatabase.rdb` cache without rescanning, and Workbench only rewrites that cache on exit. A stale cache gives `Wrong GUID/name for resource` for anything you added.
5. Validate: `mod_validate` per addon, then `node tools/validate.mjs`.
6. Build (optional): `mod_build` with `outputPath` = `<repo>\build\<addon>`.
7. Multiplayer check: [local server + clients](#local-multiplayer-dev-server--clients).
8. Autotest: `powershell -ExecutionPolicy Bypass -File tools\autotest-dev.ps1` ([details](#autotest)).
9. `git status` — no stray generated files, no missing `.meta`, no `Scripts/WorkbenchGame/EnfusionMCP/` in `addons/`.
10. Commit (Conventional Commits) and open a PR.

## DEV scenario (`NML_Scenario_Dev`)

| Resource | GUID | What |
|---|---|---|
| `Worlds/NML/Dev/NML_Dev_Everon.ent` | `{6D161EF909ABF4A7}` | Sub-scene of Everon (`{853E92315D1D9EFE}worlds/Eden/Eden.ent`). No terrain of its own. |
| `Worlds/NML/Dev/NML_Dev_Everon_Layers/default.layer` | — (layers are not registered resources) | `GameMode_Plain` (auto-spawn), `FactionManager_FFA`, `LoadoutManager_FFA`, `SpawnPoint_FFA` near 4800 / 6900. |
| `Missions/NML/NML_Dev_Everon.conf` | `{C36902459603B85D}` | `SCR_MissionHeader` for the world ("NML DEV (Everon)", 8 players). |

Vanilla prefabs only; no NML gameplay yet. Add terrain-specific data here, never in Core/Content.

## Local multiplayer (DEV server + clients)

Needs Arma Reforger Server (Steam app 1874900). Close Workbench first (step 4).

```
powershell -ExecutionPolicy Bypass -File tools\server-dev.ps1 -World "Worlds/NML/Dev/NML_Dev_Everon.ent"
powershell -ExecutionPolicy Bypass -File tools\client-dev.ps1 -Instance 1
powershell -ExecutionPolicy Bypass -File tools\client-dev.ps1 -Instance 2   # replication smoke test
```

- The server runs `ArmaReforgerServerDiag.exe -server <world> -addonsDir <repo>\addons -addons <NML_Core,NML_Content,NML_Scenario_Dev>` on UDP 2001 (A2S 17777). Pass the world as a **plain path**; a `{GUID}` prefix is sent to clients verbatim and they fail with "File not found".
- Clients run `ArmaReforgerSteamDiag.exe -client 127.0.0.1:2001` (windowed, `-noFocus`) with the same addons and their own profile in `.local\client<n>\`. The server must be the **diag** build too, or it rejects diag clients (`isDevBinary value does not match`).
- Logs: `.local\server\profile\logs\<timestamp>\console.log` and `.local\client<n>\profile\logs\...`. Success looks like `Players connected: 2 / 2` on the server and `Creating player: PlayerId=2` in client 1's log.
- Why not `-config server/configs/dev.json`? The engine refuses `-config` together with `-addons` (`-config cannot be used together with addons!`), and every `game.mods` entry in a config must be downloadable from the Workshop (`Addon was not found on workshop`). Unpublished NML addons can therefore only be served in `-server` world mode. `dev.json` becomes usable once the NML addons are on the Workshop.
- Stop with Ctrl+C in the server console, or close the windows.

## Autotest

```
powershell -ExecutionPolicy Bypass -File tools\autotest-dev.ps1                       # default: NML_TEST_DevScenarioSuite
powershell -ExecutionPolicy Bypass -File tools\autotest-dev.ps1 -Test <SuiteOrCaseClass>
```

- Runs `ArmaReforgerSteamDiag.exe -autotest <class>` with all four NML addons, waits for exit and prints the JUnit result (`.local\autotest\profile\logs\<timestamp>\junit.xml`). Exit code 0 = all passed.
- Tests live in `NML_Tests/Scripts/Game/NML/Tests/<Area>/`. Suites inherit `SCR_AutotestSuiteBase` (override `GetWorldFile()`), cases inherit `SCR_AutotestCaseBase` with `[Test(suite: …)]` and `[TestStep(TestStage.Main)]` steps. Naming: `NML_TEST_<Feature>Suite`, `NML_TEST_<Feature>_<Case>_<Expected>`.
- In Workbench: hover a suite/case class in the Script Editor → Plugins → Run test.

### Arsenal curation test world (ADR 0005)

- `NML_Tests/Worlds/NML/Tests/NML_Test_Arsenal.ent`: Everon sub-scene with vanilla US/USSR factions, one US, one USSR and one factionless arsenal, and the test arsenal policy (`NML_TEST_ArsenalPolicy_Active.conf`). Supplies are disabled on its game mode so allowed requests are not blocked by supply cost. DEV is unchanged.
- Autotest: `tools\autotest-dev.ps1 -Test NML_TEST_ArsenalPolicySuite`.
- Server enforcement proof (dedicated server + client, both with NML_Tests):
  ```
  powershell -ExecutionPolicy Bypass -File tools\server-dev.ps1 -World "Worlds/NML/Tests/NML_Test_Arsenal.ent" -IncludeTests
  powershell -ExecutionPolicy Bypass -File tools\client-dev.ps1 -Instance 1 -IncludeTests
  ```
  Spawn as US. The test-only `NML_TEST_ArsenalForgeComponent` sends a forged banned request 5 s after spawn and an allowed one at 10 s (also on demand via the diag menu "NML Tests"). Expected: server log `[NML] Rejected arsenal request … M72A3`, client log `[NML_TEST] Result: … delivered=0` for the banned item and `delivered=1` for the allowed magazine.
- `-IncludeTests` must be passed to both server and client; never use it outside test worlds.

## Local dev wrapper (`.local/NML_Dev`)

Local-only and gitignored. The Enfusion MCP copies its handler scripts into the wrapper, never into `addons/`. Each developer creates `.local/NML_Dev/NML_Dev.gproj` once; use any random 16-hex `GUID` (it is never referenced):

```
GameProject {
 ID "NML_Dev"
 GUID "<random 16 hex>"
 TITLE "NML_Dev (local wrapper, never committed)"
 Dependencies {
  "58D0FB3206B6F859"
  "C175C744D88BE5AE"
  "8C44BDA9D3046928"
  "2F33881926E82E22"
  "99E85DF2DA22A8D2"
 }
 Configurations {
  GameProjectConfig PC {
  }
  GameProjectConfig HEADLESS {
  }
 }
}
```

Dependencies, in order: base game, `NML_Core`, `NML_Content`, `NML_Scenario_Dev`, `NML_Tests`. Add new NML addons here when they are created.

## Claude Code usage

- Research before writing: Enfusion MCP `api_search`, `wiki_search`, `game_read` of the vanilla class being extended.
- Features: `/plan` first; after changes, ECC `code-review` (and `security-review` for RPCs, admin and persistence code).
- Claude works on feature branches only; never pushes to `main`, never publishes to the Workshop, never deploys servers.
- `graphify update .` after code changes (docs/config map; weak on Enforce classes).

## Dependency changes

See [dependencies/README.md](../dependencies/README.md). Summary: candidate file → evaluation → **lead approval** → registry entry + `.gproj`/server-config change in one PR.
