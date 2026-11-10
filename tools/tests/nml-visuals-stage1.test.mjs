// Tests for the NML_Visuals Stage 1 tools: probe-summary.mjs and cache-guard.mjs.
// Synthetic fixtures only; the cache-guard test works in a temp directory, never on a real Workshop cache.
// Usage: node --test tools/tests/nml-visuals-stage1.test.mjs

import { test } from "node:test";
import assert from "node:assert/strict";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { parseLine, toBlocks, pairBlocks, meanSe, summarize, readLogs } from "../nml-visuals/probe-summary.mjs";
import { snapshot, compare } from "../nml-visuals/cache-guard.mjs";
import { analyze as analyzeV1 } from "../nml-visuals/v1-log-analysis.mjs";

test("v1-log-analysis counts applies, bursts and nearest preceding causes", () =>
{
	const log = [
		"12:00:00.000 SCRIPT       : [NML] Visuals: started (Colormod 0, enabled 1, preset AUTHENTIC)",
		"12:00:00.500 SCRIPT       : [NML] Visuals: applied AUTHENTIC on camera 0 (lut x, overlays 1, lens 1)",
		"12:00:10.000 PLATFORM     : Save data transaction to 'settings' started.",
		"12:00:10.100 SCRIPT       : [NML] Visuals: applied AUTHENTIC on camera 0 (lut x, overlays 1, lens 1)",
		"12:00:10.200 SCRIPT       : [NML] Visuals: applied AUTHENTIC on camera 0 (lut x, overlays 1, lens 1)",
		"12:00:20.000 SCRIPT       : [NML] Visuals: applied AUTHENTIC on camera 1 (lut x, overlays 1, lens 1)",
		"12:00:40.000 SCRIPT       : [NML] Visuals: player set OFF (overlays 1, fringe 1)",
		"12:00:40.010 SCRIPT       : [NML] Visuals: cleared on camera 1",
		"12:01:00.000 SCRIPT       : [NML] Visuals: applied AUTHENTIC on camera 1 (lut x, overlays 1, lens 1)",
	].join("\n");
	const r = analyzeV1(log, 2000);
	assert.equal(r.applied, 5);
	assert.equal(r.cleared, 1);
	assert.equal(r.burstApplies, 1);
	assert.deepEqual(r.cameras, [0, 1]);
	assert.deepEqual(r.byCause, { WORLD: 1, SETTINGS_SAVE: 2, CAMERA_INDEX: 1, USER: 1, UNATTRIBUTED: 1 });
	assert.equal(Math.round(r.minutes), 1);
});

const line = (phase, ms, frames) =>
	`12:00:00.000  SCRIPT       : [NML] Probe: phase=${phase} ms=${ms} frames=${frames} mean100=${Math.round((ms * 100) / frames)} p50=16 p95=20 p99=30 max=40 s33=1 s50=0 s100=0 aSpk=0 aMax=0 applies=0,0,0,0,0,0,0,1 sets=3 clears=3 reasserts=0 cam=0 ctl=0 setOurs=0 setOther=0 starts=0 layers=lut+trim mode=legacy fringeSlot=13 filter=0 hideZero=0 preset=AUTHENTIC night=0 view=1P players=1`;

test("parseLine reads numbers, applies and text fields", () =>
{
	const r = parseLine(line("A:off", 10000, 600));
	assert.equal(r.phase, "A:off");
	assert.equal(r.ms, 10000);
	assert.equal(r.frames, 600);
	assert.deepEqual(r.applies, [0, 0, 0, 0, 0, 0, 0, 1]);
	assert.equal(r.layers, "lut+trim");
	assert.equal(r.view, "1P");
	assert.equal(parseLine("unrelated line"), null);
});

test("toBlocks merges intervals of one phase and splits on newPhase", () =>
{
	const recs = [line("A:off", 10000, 600), line("A:off", 10000, 600), line("B:on", 10000, 500), line("B:on", 10000, 500)].map(parseLine);
	recs[3].newPhase = true; // ABBA: B then B again
	const blocks = toBlocks(recs);
	assert.equal(blocks.length, 3);
	assert.equal(blocks[0].frames, 1200);
	assert.equal(blocks[1].frames, 500);
	assert.equal(blocks[2].frames, 500);
});

test("pairBlocks pairs A/B neighbours in ABBA order", () =>
{
	const mk = (phase, ms, frames) => ({ phase, ms, frames });
	const pairs = pairBlocks([mk("A:off", 1000, 100), mk("B:on", 1100, 100), mk("B:on", 1100, 100), mk("A:off", 1000, 100)]);
	assert.equal(pairs.length, 2);
	for (const p of pairs)
		assert.ok(Math.abs(p.delta - 1) < 1e-9);
});

test("meanSe handles 0, 1 and n values", () =>
{
	assert.ok(Number.isNaN(meanSe([]).mean));
	assert.ok(Number.isNaN(meanSe([1]).se));
	const r = meanSe([1, 2, 3]);
	assert.equal(r.mean, 2);
	assert.ok(Math.abs(r.se - 1 / Math.sqrt(3)) < 1e-9);
});

test("summarize reports a clear difference and no difference", () =>
{
	// 4 pairs, B consistently ~1.6 ms slower with small noise
	const big = [];
	[0.0, 0.1, -0.1, 0.05].forEach((n, i) =>
	{
		const a = parseLine(line("A:off", 16000, 1000));
		const b = parseLine(line("B:on", Math.round((17.6 + n) * 1000), 1000));
		a.newPhase = b.newPhase = true;
		if (i % 2 === 0) big.push(a, b); else big.push(b, a);
	});
	const s = summarize(big);
	assert.equal(s.pairs, 4);
	assert.ok(s.deltaMs > 1.5 && s.deltaMs < 1.7);
	assert.match(s.verdict, /^differs/);

	// Noise larger than the (zero) effect -> no difference
	const flat = [];
	[0.5, -0.5, 0.6, -0.6].forEach((n) =>
	{
		const a = parseLine(line("A:off", 16000, 1000));
		const b = parseLine(line("B:on", Math.round((16 + n) * 1000), 1000));
		a.newPhase = b.newPhase = true;
		flat.push(a, b);
	});
	assert.match(summarize(flat).verdict, /^no difference/);
});

test("readLogs marks the record after a cycle-phase line as a new phase", () =>
{
	const dir = fs.mkdtempSync(path.join(os.tmpdir(), "nmlv-"));
	const log = path.join(dir, "console.log");
	fs.writeFileSync(log, [
		"x [NML] Visuals: cycle phase 1 B:on",
		line("B:on", 10000, 500),
		"x [NML] Visuals: cycle phase 2 B:on",
		line("B:on", 10000, 500),
	].join("\n"));
	const recs = readLogs([log]);
	assert.equal(recs.length, 2);
	assert.equal(toBlocks(recs).length, 2);
	fs.rmSync(dir, { recursive: true, force: true });
});

test("cache-guard detects added, removed and changed files", () =>
{
	const root = fs.mkdtempSync(path.join(os.tmpdir(), "nmlcache-"));
	const mod = path.join(root, "ModA_0000000000000001");
	fs.mkdirSync(mod);
	fs.writeFileSync(path.join(mod, "data.pak"), "aaaa");
	fs.writeFileSync(path.join(mod, "old.json"), "x");
	const before = snapshot(root);
	assert.deepEqual(compare(before, snapshot(root)), { added: [], removed: [], changed: [] });

	fs.writeFileSync(path.join(mod, "data.pak"), "aaaaaaaa");
	fs.rmSync(path.join(mod, "old.json"));
	fs.writeFileSync(path.join(mod, "new.json"), "y");
	const d = compare(before, snapshot(root));
	assert.deepEqual(d.changed, ["ModA_0000000000000001/data.pak"]);
	assert.deepEqual(d.removed, ["ModA_0000000000000001/old.json"]);
	assert.deepEqual(d.added, ["ModA_0000000000000001/new.json"]);
	fs.rmSync(root, { recursive: true, force: true });
});
