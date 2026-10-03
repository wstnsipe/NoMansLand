# NML mod-candidate research review

**Research snapshot: 2026-10-02.** Updated from the original 2026-10-01 intake review. This is member-directed documentation on PR #6, not lead approval, candidate promotion, Workshop registration, or implementation.

## Current research documents

- [Complete 87-package mod inventory](2026-10-02-mod-inventory.md): member status, Workshop IDs, installed versions, project GUID evidence, purpose, direct/transitive dependencies, testing status and compatibility notes; also records excluded/parked/optional candidates.
- [Arsenal, resource and testing handoff](2026-10-02-arsenal-status.md): confirmed preferences versus recommendations/ideas, VT4 and Myrove discovery, exact known resources, limitations and Weston decisions.
- [Original member intake](../../dependencies/candidates/intake/2026-09-30-batch-1.md) remains historical input; its contents and candidate lifecycle have not changed.

The [AI operating rules](../AI_OPERATING_RULES.md), [workboard](../AI_WORKBOARD.md), and accepted ADRs remain authoritative. These research documents supersede older camera choices and installation statements in the first version of this PR.

## Current member preferences

- **Camera/movement:** CERTII-SimulatedMovement + Best Body Cam. CERTII-Minimal-HUD and CERTII-Grenade-Limiter retained. **SlavicWar Bodycam excluded**, as is Immersive Head Movement. Keep existing intensity settings; the cancelled Slavic comparison does not authorize changing them.
- **Medical:** matching ACE Dev Core/Medical Core/Hitzones/Circulation/Breathing plus SplintACE. Medical AI Dev is optional, not selected.
- **Combat effects:** GC Suppression, 2-7 Overheating, Realistic Combat Sound Mod, Screams of War, Improved Blood Effect Deluxe plus its base.
- **Equipment:** newer Minnesinger Clothes/Core/Gear/Headwear + Zeliks. Older family/collection/RHS patch parked. Both Ukraine equipment packs retained for comparison.
- **Additional requested weapons:** MCR-Pack 5.56x45 and FN SCAR Heavy/Light. MCR brings RIS Laser Attachments and retained Bacon Suppressors.
- **Ranks:** FTA retained as a member-selected evaluation candidate; thresholds, rewards, resets and NML integration are unresolved.
- **VT4:** brown BTDF + Equipped proposed for Ukrainian-inspired side; green CE + Equipped for Russian-inspired side. This is not a configured or approved NML faction allocation.

## Installation and dependency findings

- 87 unique packages, 68 current candidate entries + 19 additional dependencies.
- **86/87** pass the local GUID/payload/manifest-size audit. **Discord Admin Stats (`68C61B955A988F7E`) is missing.**
- Installed project GUIDs match their Workshop IDs. Version numbers are local snapshots, not promises of latest Workshop releases.
- Direct project relationships have been separated from the transitive closure that Workshop pages sometimes expose. Newer Minnesinger Headwear directly requires NV-System; that package and its chain are already installed.
- FORTEX native heating/degradation and selected 2-7 heating remain an unresolved overlap, not a proven runtime conflict.
- SW-Reap-IR brings a faction-replacement dependency. RTS Protocol public-server and SW-Reap-IR server authorization requirements remain unresolved.

## Gameplay evidence

- Member reported a successful VT4 BTDF drive test in FritV3. The local child and world instances exist; logs include material/editor warnings and errors.
- Member reports CERTII + Best Body Cam already work together. Scenario/settings and full-stack evidence are not recorded; Codex has not reproduced the test.
- Earlier supplied handoff records a failed save into a read-only Bacon dependency config. This is not evidence of a broken gameplay mod.
- No complete candidate-pack, NML arsenal, dedicated-server, two-client or performance test is claimed. No new gameplay testing is performed by this publication.

## Arsenal and approval boundary

**No finished item-level whitelist exists.** Mod preferences are content sources for evaluation, not approved weapon/loadout lists.

[ADR 0005](../adr/0005-arsenal-curation.md) remains accepted: the first implementation is vanilla-only, faction-keyed by the arsenal, with server enforcement and an empty shipped policy. Scratch overwrite-config/Bacon ideas do not replace that design. [ADR 0004](../adr/0004-medical-boundary.md) retains its medical provider boundary.

Stage 5.3 has not started and remains blocked until Weston explicitly approves it. Current registry remains unchanged and empty. No addons, project dependencies, server configurations, accepted ADRs, architecture, production settings or third-party assets are changed by this research PR.

## Ownership and overlap

Scope is publication of the existing member-directed Codex research on `chore/mod-candidate-review-2026-10-01`, PR #6, under `docs/research/` only. Current main, operating rules, workboard and relevant open PRs/issues were checked; no conflicting active research-file owner or other open PR was found. Myrove evaluation remains planned with owner TBD; this records existing discovery only. Weston retains implementation/integration authority. Workboard ownership and task locks are unchanged.

Before production integration, Weston must approve the final dependency set/versions, equipment and faction policies, medical/progression approach, permissions, and required compatibility/performance tests. See the handoff for unresolved details. Contributor merge remains subject to Weston review; merging remains Weston's decision.
