# Contributing to NML

## One-time setup

1. Install Git LFS, Node 18+, and Arma Reforger Tools (Steam). Enable Workbench's Net API (File > Options > General > Net API, port 5775) if you use the Enfusion MCP.
2. Clone, then run `powershell -ExecutionPolicy Bypass -File tools\install-hooks.ps1` (enables LFS for this repo and installs the pre-commit validator).
3. Run `node tools/validate.mjs` — it must report 0 errors.

## Branches and PRs

- Never push to `main`. Work on short-lived branches: `feat/<system>-<desc>`, `fix/…`, `chore/…`, `content/…`, `deps/…`.
- Commit messages follow Conventional Commits: `feat(loot): add container respawn timer`.
- A PR needs: green CI, one approving review, and the CODEOWNERS reviewer for touched paths. Squash merge (the only merge method enabled).
- **`main` is not protected on GitHub yet.** `wstnsipe/NoMansLand` is a private repo on GitHub Free, where branch protection and rulesets are unavailable. The rules above are enforced by convention, CI, the pre-commit validator and the Claude Code guard hook, not by GitHub. Enable branch protection (PRs, reviews, conversation resolution, no force-push/deletion, linear history) when collaborators join or the account is upgraded.
- Fill in the PR template, especially the `.meta` / GUID and dependency questions.

## Enfusion conventions

- Tag everything with `NML_`: classes `NML_LootComponent`, enums `NML_ELootType`, files `NML_LootComponent.c`, addons `NML_Core`.
- Allman braces, tab indentation, members `m_` + type letter (`m_iCount`), components end in `Component`, entities in `Entity`.
- Scripts live in `Scripts/Game/NML/<System>/`. `modded class` overrides of vanilla live **only** in `Scripts/Game/NML/Modded/`, and every member you add there is `NML_`-prefixed.
- Namespace content under `NML/` (e.g. `Prefabs/NML/…`, `Configs/NML/…`).
- Never delete or regenerate `.meta` files; rename and move resources inside Workbench so the `.meta` moves too.
- Worlds: work in your own layer / sub-scene; do not edit a layer someone else owns.
- Binary assets are LFS-tracked: `git lfs lock <file>` before editing, unlock after merge.

## Local checks

```
node tools/validate.mjs              # repository rules (also runs on pre-commit and CI)
node tools/tests/validate.test.mjs   # validator self-tests
node tools/tests/guard.test.mjs      # Claude Code guard-hook tests
node tools/candidates-index.mjs      # regenerate dependencies/candidates/INDEX.md
```

## Third-party mods

Never add a mod to a `.gproj` or `dependencies/mods.json` without an approved candidate. See [dependencies/README.md](dependencies/README.md).
