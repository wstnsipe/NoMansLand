# addons/

Each subfolder is one Enfusion addon with its own `.gproj`, GUID and (if published) Workshop item. Folder name = `.gproj` ID and starts with `NML_`.

| Addon | Contents | Depends on | Published |
|---|---|---|---|
| `NML_Core` | Scripts, configs, game mode, UI, string tables, persistence config. No terrain references. | base game + approved core deps | Yes |
| `NML_Content` | Prefabs, models, textures, sounds, catalogs | NML_Core | Yes |
| `NML_Scenario_Dev` | Mission header + sub-scene on a base-game terrain for DEV/TEST | Core, Content | No |
| `NML_Tests` | Autotest suites and test worlds | Core, Content, Scenario_Dev (tests load the DEV world/mission) | Never |
| `NML_Compat_<Mod>` | Integration with one approved third-party mod | Core + that mod | If needed |
| `NML_Scenario_Myrove` (later) | Scenario on the Myrove terrain | Core, Content, Myrove (external Workshop dependency) | Yes |

Addons are created in Phase 2 with `mod_create` (explicit `projectPath` = this folder). The local dev wrapper `NML_Dev` lives in `.local/` (gitignored), not here. See [docs/architecture.md](../docs/architecture.md).
