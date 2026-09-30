## What and why

<!-- One or two sentences. Link the issue. -->

## Area

- [ ] NML_Core scripts / configs
- [ ] NML_Content assets / prefabs
- [ ] Scenario / world layers
- [ ] Dependencies / server configs
- [ ] Tooling / docs

## Checks

- [ ] `node tools/validate.mjs` passes locally
- [ ] `mod_validate` passes for touched addons
- [ ] Tested in Workbench Play mode (and on a local dedicated server if replication changed)
- [ ] No `Scripts/WorkbenchGame/EnfusionMCP/` in `addons/`

## Enfusion specifics

- [ ] New/moved resources have their `.meta` files committed
- [ ] **Any GUID changed?** If yes, explain why:
- [ ] **Any `.gproj` dependency or server-config mod changed?** If yes, link the approved candidate:
- [ ] **Persistence / save format changed?** If yes, migration or wipe note:
- [ ] New RPCs validate range / cooldown / ownership / state on the server
