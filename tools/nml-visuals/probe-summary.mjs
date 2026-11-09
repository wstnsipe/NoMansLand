#!/usr/bin/env node
// Summarise NML_Visuals Stage 1 probe logs (docs/visuals/v2/STAGE1-PLAN.md).
//
// Reads "[NML] Probe: key=value ..." lines from game console logs, groups consecutive intervals into cycle
// phases (-nmlvCycle, labels "A:..." / "B:..."), pairs each phase with its neighbour (ABBA order) and reports
// the mean frame-time difference B - A with its standard error across pairs, spike rates and apply counts.
// Verdict: "differs" when |delta| >= max(2, t 95 %) x SE over >= 3 pairs (the plan's 2 x SE, made stricter
// for few pairs), otherwise "no difference".
//
// Usage: node tools/nml-visuals/probe-summary.mjs <console.log> [more logs...] [--json]

import fs from "node:fs";
import { fileURLToPath } from "node:url";

export const REASONS = ["WORLD", "CAMERA", "CONTROLLED", "LOWLIGHT", "SETTINGS_OURS", "SETTINGS_OTHER", "USER", "CYCLE"];
const NUMERIC = ["ms", "frames", "mean100", "p50", "p95", "p99", "max", "s33", "s50", "s100", "aSpk", "aMax", "sets", "clears", "reasserts", "cam", "ctl", "setOurs", "setOther", "starts", "players"];

/** Parses one log line; returns null when it is not a probe summary line. */
export function parseLine(line)
{
	const at = line.indexOf("[NML] Probe: phase=");
	if (at < 0)
		return null;

	const rec = {};
	for (const token of line.slice(at + "[NML] Probe: ".length).trim().split(/\s+/))
	{
		const eq = token.indexOf("=");
		if (eq > 0)
			rec[token.slice(0, eq)] = token.slice(eq + 1);
	}

	for (const key of NUMERIC)
		if (key in rec)
			rec[key] = Number(rec[key]);

	rec.applies = (rec.applies || "").split(",").map(Number);
	return rec;
}

/**
 * Merges intervals into phase blocks. A block ends when the label changes or when the record carries
 * newPhase (a cycle switch between two phases with the same label: ABBA's B,B and A,A).
 */
export function toBlocks(records)
{
	const blocks = [];
	for (const r of records)
	{
		const last = blocks[blocks.length - 1];
		if (!last || last.phase !== r.phase || r.newPhase)
			blocks.push({ phase: r.phase, ms: 0, frames: 0, s33: 0, s50: 0, s100: 0, aSpk: 0, max: 0, applies: REASONS.map(() => 0), intervals: 0 });

		const b = blocks[blocks.length - 1];
		b.ms += r.ms;
		b.frames += r.frames;
		b.s33 += r.s33 || 0;
		b.s50 += r.s50 || 0;
		b.s100 += r.s100 || 0;
		b.aSpk += r.aSpk || 0;
		b.max = Math.max(b.max, r.max || 0);
		r.applies.forEach((n, i) => { if (i < b.applies.length) b.applies[i] += n || 0; });
		b.intervals++;
	}

	return blocks;
}

const arm = (phase) => (phase.startsWith("A:") ? "A" : phase.startsWith("B:") ? "B" : null);

/** Pairs neighbouring A/B blocks, (A,B) or (B,A); each block is used once. */
export function pairBlocks(blocks)
{
	const cyc = blocks.filter((b) => arm(b.phase) && b.frames > 0);
	const pairs = [];
	for (let i = 0; i + 1 < cyc.length; i++)
	{
		const x = cyc[i], y = cyc[i + 1];
		if (arm(x.phase) === arm(y.phase))
			continue;

		const a = arm(x.phase) === "A" ? x : y;
		const b = a === x ? y : x;
		pairs.push({ a, b, delta: b.ms / b.frames - a.ms / a.frames });
		i++;
	}

	return pairs;
}

// Two-sided 95 % Student-t critical values for df = 1..30. The plan's "2 x SE" rule is used as the floor, so
// with few pairs the stricter t value applies.
const T95 = [12.706, 4.303, 3.182, 2.776, 2.571, 2.447, 2.365, 2.306, 2.262, 2.228, 2.201, 2.179, 2.160, 2.145, 2.131, 2.120, 2.110, 2.101, 2.093, 2.086, 2.080, 2.074, 2.069, 2.064, 2.060, 2.056, 2.052, 2.048, 2.045, 2.042];

/** Multiple of SE that |delta| must reach for n pairs: max(2, t(0.975, n - 1)). */
export function critical(n)
{
	if (n < 2)
		return Infinity;

	return Math.max(2, T95[Math.min(n - 1, T95.length) - 1]);
}

export function meanSe(values)
{
	const n = values.length;
	if (n === 0)
		return { n, mean: NaN, se: NaN };

	const mean = values.reduce((s, v) => s + v, 0) / n;
	if (n === 1)
		return { n, mean, se: NaN };

	const variance = values.reduce((s, v) => s + (v - mean) ** 2, 0) / (n - 1);
	return { n, mean, se: Math.sqrt(variance / n) };
}

/** Full summary for a list of probe records. */
export function summarize(records)
{
	const blocks = toBlocks(records);
	const pairs = pairBlocks(blocks);
	const d = meanSe(pairs.map((p) => p.delta));

	const sumArm = (k) =>
	{
		const sel = blocks.filter((b) => arm(b.phase) === k);
		const frames = sel.reduce((s, b) => s + b.frames, 0);
		const ms = sel.reduce((s, b) => s + b.ms, 0);
		return {
			label: sel[0]?.phase ?? "",
			frames,
			meanMs: frames ? ms / frames : NaN,
			s33Per1k: frames ? (1000 * sel.reduce((s, b) => s + b.s33, 0)) / frames : NaN,
			s100Per1k: frames ? (1000 * sel.reduce((s, b) => s + b.s100, 0)) / frames : NaN,
		};
	};

	const applies = blocks.reduce((acc, b) => { b.applies.forEach((n, i) => (acc[i] += n)); return acc; }, REASONS.map(() => 0));
	const applySpikes = blocks.reduce((s, b) => s + b.aSpk, 0);
	const totalFrames = blocks.reduce((s, b) => s + b.frames, 0);
	const totalS33 = blocks.reduce((s, b) => s + b.s33, 0);
	const nonCycleApplies = applies.reduce((s, n, i) => (REASONS[i] === "CYCLE" ? s : s + n), 0);

	const A = sumArm("A"), B = sumArm("B");
	const k = critical(d.n);
	let verdict = "insufficient pairs (< 3)";
	if (d.n >= 3)
	{
		if (!(d.se > 0))
			verdict = "no spread between pairs (check the log)";
		else if (Math.abs(d.mean) >= k * d.se)
			verdict = `differs (|delta| >= ${k.toFixed(2)} SE)`;
		else
			verdict = `no difference (|delta| < ${k.toFixed(2)} SE)`;
	}

	return {
		pairs: d.n,
		deltaMs: d.mean,
		seMs: d.se,
		deltaPct: A.meanMs ? (100 * d.mean) / A.meanMs : NaN,
		verdict,
		A,
		B,
		applies: Object.fromEntries(REASONS.map((r, i) => [r, applies[i]])),
		applySpikes,
		// H4: spikes right after non-CYCLE applies vs the background spike rate
		backgroundSpikePer1k: totalFrames ? (1000 * (totalS33 - applySpikes)) / totalFrames : NaN,
		applySpikesPerApply: nonCycleApplies ? applySpikes / nonCycleApplies : NaN,
	};
}

/** Reads probe records from log files; a "cycle phase" line marks the next record as a new phase. */
export function readLogs(paths)
{
	const records = [];
	for (const p of paths)
	{
		let pendingPhase = false;
		for (const line of fs.readFileSync(p, "utf8").split(/\r?\n/))
		{
			if (line.includes("[NML] Visuals: cycle phase"))
			{
				pendingPhase = true;
				continue;
			}

			const rec = parseLine(line);
			if (!rec)
				continue;

			// The probe flushes the old phase on the switch, so the first record after the marker is the new phase.
			rec.newPhase = pendingPhase;
			pendingPhase = false;
			records.push(rec);
		}
	}

	return records;
}

const fmt = (v, digits = 2) => (Number.isFinite(v) ? v.toFixed(digits) : "n/a");

if (process.argv[1] && fileURLToPath(import.meta.url) === process.argv[1])
{
	const args = process.argv.slice(2);
	const json = args.includes("--json");
	const files = args.filter((a) => a !== "--json");
	if (files.length === 0)
	{
		console.error("Usage: node tools/nml-visuals/probe-summary.mjs <console.log> [...] [--json]");
		process.exit(2);
	}

	const s = summarize(readLogs(files));
	if (json)
	{
		console.log(JSON.stringify(s, null, 2));
	}
	else
	{
		console.log(`A ${s.A.label}: mean ${fmt(s.A.meanMs)} ms, ${s.A.frames} frames, spikes>33 ${fmt(s.A.s33Per1k)}/1k, >100 ${fmt(s.A.s100Per1k)}/1k`);
		console.log(`B ${s.B.label}: mean ${fmt(s.B.meanMs)} ms, ${s.B.frames} frames, spikes>33 ${fmt(s.B.s33Per1k)}/1k, >100 ${fmt(s.B.s100Per1k)}/1k`);
		console.log(`B - A: ${fmt(s.deltaMs, 3)} ms (${fmt(s.deltaPct, 1)} %), SE ${fmt(s.seMs, 3)} over ${s.pairs} pairs -> ${s.verdict}`);
		console.log(`applies: ${Object.entries(s.applies).filter(([, n]) => n).map(([r, n]) => `${r} ${n}`).join(", ") || "none"}`);
		console.log(`after-apply spikes: ${s.applySpikes} (${fmt(s.applySpikesPerApply)} per non-cycle apply); background ${fmt(s.backgroundSpikePer1k)}/1k frames`);
	}
}
