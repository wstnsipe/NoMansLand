# Contributing to NML

## One-time setup

1. Install Git LFS, Node 18+, Arma Reforger Tools and Arma Reforger Server (Steam app 1874900, for local multiplayer tests). Enable Workbench's Net API (File > Options > General > Net API, port 5775) if you use the Enfusion MCP.
2. Clone, then run `powershell -ExecutionPolicy Bypass -File tools\install-hooks.ps1` (enables LFS for this repo and installs the pre-commit validator).
3. Run `node tools/validate.mjs` — it must report 0 errors.

## Branches and PRs

- Never push to `main`. Work on short-lived branches: `feat/<system>-<desc>`, `fix/…`, `chore/…`, `content/…`, `deps/…`.
- Commit messages follow Conventional Commits: `feat(loot): add container respawn timer`.
- A PR needs green CI (run by convention; not yet a required status check) and resolved conversations. Squash merge is the only merge method enabled. Once collaborators join, PRs also need one approving review and the CODEOWNERS reviewer for touched paths.
- **`main` is protected on GitHub** by the `protect-main` ruleset (public repo): changes only through a pull request, conversations must be resolved, no force-push, no branch deletion, linear history, squash merge only, no bypass actors. Approving reviews are **not required yet** because there is a single active developer. When collaborators join, raise `required_approving_review_count` to 1, turn on "require code owner review" and make the `validate` CI check required.
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
