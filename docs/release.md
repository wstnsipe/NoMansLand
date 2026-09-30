# Release process

## Versioning

- SemVer `X.Y.Z`. `NML_Core` and `NML_Content` release in **lockstep**; git tag `vX.Y.Z` = Workshop version of both (Workshop format `X.Y.Z`, each part ≤ 32000).
- **MAJOR:** persistence / save-format break or wipe. **MINOR:** features. **PATCH:** fixes.
- Scenario and map addons version independently; the registry records compatible NML and map versions.

## Steps

1. Cut `release/X.Y` from `main`; only fixes are cherry-picked onto it.
2. Update `CHANGELOG.md` (from Conventional Commits). Flag wipes and persistence migrations.
3. Build with `mod_build` (`outputPath` under `build/`), confirm `node tools/validate.mjs --ci` is green, confirm no `Scripts/WorkbenchGame/EnfusionMCP/` exists in any addon.
4. Publish the release candidate to the Workshop (Workbench > Publish Project; Contributors can publish updates). The publish working dir stays outside the repo (default `Documents\My Games\ArmaReforger\publish\<addon>`).
5. Pin the new version in the TEST server config; soak test using the checklist below.
6. Tag `vX.Y.Z`, create the GitHub Release, pin the version in the LIVE config.

## TEST soak checklist

- [ ] Server starts with all pinned mods; no script errors in the server log
- [ ] Two+ clients join; replication of core systems verified
- [ ] Persistence: save, restart, load — state intact (or wipe documented)
- [ ] Admin tools work; no client can trigger server outcomes directly
- [ ] Performance: server FPS and tick within budget with expected player count
- [ ] Rollback plan: previous version still pinned-able

## Workshop notes

- The Workshop keeps the last 50 versions of an item.
- Visibility options for release candidates are unconfirmed — check the Publish dialog on the first publish.
- Experimental Workshop is separate from stable.
