# Candidate backlog

- One file per evaluated candidate: `<slug>.md`, copied from [_TEMPLATE.md](_TEMPLATE.md).
- Raw batches from members go into [intake/](intake/) first; a candidate gets its own file when it is evaluated in depth.
- [INDEX.md](INDEX.md) is generated: `node tools/candidates-index.mjs` (CI checks it is current).
- Status `approved` / `registered` may only be set by the lead (`approvedBy` required). Claude never sets them.
- The backlog is open-ended; absence of a mod or category does not mean the decision is made.
