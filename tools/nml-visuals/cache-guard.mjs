#!/usr/bin/env node
// Cache guard for NML_Visuals Stage 1 test sessions (docs/visuals/v2/STAGE1-PLAN.md, step 0).
//
// Takes a read-only snapshot of a Workshop addon cache (every addon folder's top-level files: name, size,
// modified time; addons keep their data.pak, manifests and .gproj at that level, so subfolders are not walked)
// and compares two snapshots. Run "snapshot" before and after every local session that loads
// mods from the player's real cache; "compare" exits 1 on any change so the session can be stopped and
// reported. Never writes into the cache; snapshots go to the path given (keep them under .local/).
//
// Usage:
//   node tools/nml-visuals/cache-guard.mjs snapshot <cacheAddonsDir> <out.json>
//   node tools/nml-visuals/cache-guard.mjs compare <before.json> <after.json>

import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

export function snapshot(root)
{
	const entries = {};
	for (const dir of fs.readdirSync(root, { withFileTypes: true }))
	{
		if (!dir.isDirectory())
			continue;

		const full = path.join(root, dir.name);
		for (const f of fs.readdirSync(full, { withFileTypes: true }))
		{
			if (!f.isFile())
				continue;

			const st = fs.statSync(path.join(full, f.name));
			entries[`${dir.name}/${f.name}`] = [st.size, Math.round(st.mtimeMs)];
		}
	}

	return { root, takenAt: new Date().toISOString(), entries };
}

/** Returns { added, removed, changed } path lists. */
export function compare(before, after)
{
	const added = [], removed = [], changed = [];
	for (const [k, v] of Object.entries(after.entries))
	{
		if (!(k in before.entries))
			added.push(k);
		else if (before.entries[k][0] !== v[0] || before.entries[k][1] !== v[1])
			changed.push(k);
	}

	for (const k of Object.keys(before.entries))
		if (!(k in after.entries))
			removed.push(k);

	return { added, removed, changed };
}

if (process.argv[1] && fileURLToPath(import.meta.url) === process.argv[1])
{
	const [cmd, a, b] = process.argv.slice(2);
	if (cmd === "snapshot" && a && b)
	{
		const rel = path.relative(path.resolve(a), path.resolve(b));
		if (!rel.startsWith("..") && !path.isAbsolute(rel))
		{
			console.error("cache guard: refusing to write the snapshot inside the cache it guards");
			process.exit(2);
		}

		const s = snapshot(a);
		fs.mkdirSync(path.dirname(path.resolve(b)), { recursive: true });
		fs.writeFileSync(b, JSON.stringify(s));
		console.log(`snapshot: ${Object.keys(s.entries).length} files in ${a} -> ${b}`);
	}
	else if (cmd === "compare" && a && b)
	{
		const d = compare(JSON.parse(fs.readFileSync(a, "utf8")), JSON.parse(fs.readFileSync(b, "utf8")));
		const n = d.added.length + d.removed.length + d.changed.length;
		if (n === 0)
		{
			console.log("cache guard: no change");
		}
		else
		{
			console.log(`cache guard: ${n} change(s) - STOP and report`);
			for (const k of d.added) console.log(`  added   ${k}`);
			for (const k of d.removed) console.log(`  removed ${k}`);
			for (const k of d.changed) console.log(`  changed ${k}`);
			process.exit(1);
		}
	}
	else
	{
		console.error("Usage: cache-guard.mjs snapshot <cacheAddonsDir> <out.json> | compare <before.json> <after.json>");
		process.exit(2);
	}
}
