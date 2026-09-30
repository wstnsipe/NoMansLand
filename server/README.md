# Server configs

| File | Env | Notes |
|---|---|---|
| `configs/dev.json` | DEV | Localhost, not visible, no secrets. Mods resolve from the checkout via `-addonsDir`. |
| `configs/test.template.json` | TEST | Every mod **pins** `version`. Secrets are `${PLACEHOLDERS}`. |
| `configs/live.template.json` | LIVE | Same rules; no Dev-channel mods unless approved in the registry. |

## Rules (enforced by `tools/validate.mjs`)

- Mod entries are `{ "modId", "name", "version", "required" }`. `name` is only a label. An omitted `version` means **latest** — TEST/LIVE must pin it.
- Every mod is either an NML addon or an approved entry in `dependencies/mods.json`.
- No literal passwords, tokens or admin IDs. Use `${NML_…}` placeholders; real values are filled on the server host (never committed; `*.local.json` and `*.secret.json` are gitignored).
- Every listed mod is a required client download — watch the size report printed by the validator.

## Deployment

Manual, by a human, on the server host. Claude Code never deploys and never holds production credentials. Default ports: 2001 (game, UDP), 17777 (A2S), 19999 (RCON). Keep BattlEye enabled and limit `-maxFPS` on public servers.

The NML addon GUIDs are added to these configs in Phase 2 once the addons exist.
