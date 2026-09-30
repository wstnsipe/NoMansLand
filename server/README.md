# Server configs

| File | Env | Notes |
|---|---|---|
| `configs/dev.json` | DEV | Localhost, not visible, no secrets. Scenario = `NML DEV (Everon)` (`{C36902459603B85D}Missions/NML/NML_Dev_Everon.conf`). DEV runs the local unpacked addons from the checkout, so `mods` stays empty; Workshop mod entries (`modId`, `name`) are added after the NML addons are published. Until then the local server runs in world mode — see below. |
| `configs/test.template.json` | TEST | Every mod **pins** `version`. Secrets are `${PLACEHOLDERS}`. |
| `configs/live.template.json` | LIVE | Same rules; no Dev-channel mods unless approved in the registry. |

## Rules (enforced by `tools/validate.mjs`)

- Mod entries are `{ "modId", "name", "version", "required" }`. `name` is only a label. An omitted `version` means **latest** — TEST/LIVE must pin it.
- Every mod is either an NML addon or an approved entry in `dependencies/mods.json`.
- No literal passwords, tokens or admin IDs. Use `${NML_…}` placeholders; real values are filled on the server host (never committed; `*.local.json` and `*.secret.json` are gitignored).
- Every listed mod is a required client download — watch the size report printed by the validator.

## Deployment

Manual, by a human, on the server host. Claude Code never deploys and never holds production credentials. Default ports: 2001 (game, UDP), 17777 (A2S), 19999 (RCON). Keep BattlEye enabled and limit `-maxFPS` on public servers.

## Local DEV server (unpublished NML addons)

A config file cannot serve unpublished addons: every `game.mods` entry must exist on the Workshop (`Addon was not found on workshop`), and `-config` cannot be combined with `-addons` (`-config cannot be used together with addons!`). Until the NML addons are published, the local DEV server therefore runs in world mode, without `dev.json`:

```
powershell -ExecutionPolicy Bypass -File tools\server-dev.ps1 -World "Worlds/NML/Dev/NML_Dev_Everon.ent"
```

It starts `ArmaReforgerServerDiag.exe -server <world> -addonsDir <repo>\addons -addons C175C744D88BE5AE,8C44BDA9D3046928,2F33881926E82E22` (NML_Core, NML_Content, NML_Scenario_Dev; NML_Tests never runs on a server). The profile and logs go to the gitignored `.local\server\profile`. Clients: `tools\client-dev.ps1`. Full workflow: [docs/workflows.md](../docs/workflows.md#local-multiplayer-dev-server--clients).

`tools\server-dev.ps1` without `-World` runs `-config server\configs\dev.json` (for published builds).
