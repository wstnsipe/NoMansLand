#!/usr/bin/env node
// Re-apply analysis for NML_Visuals v1.0.0 client logs (Stage 1 evidence pass, docs/visuals/v2/STAGE1-RESULTS.md).
//
// v1 logs every slot write as "[NML] Visuals: applied <PRESET> on camera <n> (...)" or "... cleared on camera <n>",
// but not why. This tool counts them per session, measures bursts, and attributes each one to the nearest
// preceding event within a window (default 2 s) from the same log. Attribution is an inference from
// proximity, not proof of cause.
//
// Usage: node tools/nml-visuals/v1-log-analysis.mjs <console.log> [more logs...] [--window <ms>] [--json]

import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

// Event categories, checked in this order; the first match wins for a line.
export const EVENTS = [
	["USER", /\[NML\] Visuals: (player set|key ->|settings changed|cycle ->)/],
	["SETTINGS_SAVE", /Save data transaction to 'settings'|ChangeEngineUserSettings|Settings changed took/],
	["WORLD", /\[NML\] Visuals: started|Loading world|Game successfully created|Entered online game state/],
	["PLAYER", /Updating player:|Players connected:|OnControlledEntityChanged/],
	["MENU", /Creating menu '/],
	["COLORMOD_RELOAD", /Material load @"\{7C202A913EB8B1A9\}UI\/Materials\/ScreenEffects_ColorPP\.emat"/],
	["DISCONNECT", /Disconnect/],
];

const TIME = /^(\d{2}):(\d{2}):(\d{2})\.(\d{3})/;
const APPLY = /\[NML\] Visuals: (applied (\S+) on camera (-?\d+)|cleared on camera (-?\d+))/;

export function toMs(line)
{
	const m = TIME.exec(line);
	if (!m)
		return null;

	return ((Number(m[1]) * 60 + Number(m[2])) * 60 + Number(m[3])) * 1000 + Number(m[4]);
}

/** Analyses one log's text. Returns a session summary. */
export function analyze(text, windowMs = 2000)
{
	const events = []; // { t, kind }
	const applies = []; // { t, kind: applied|cleared, camera, preset, cause }
	let first = null, last = null, dayOffset = 0, prevT = null;

	for (const line of text.split(/\r?\n/))
	{
		let t = toMs(line);
		if (t === null)
			continue;

		// Logs can cross midnight: keep time monotonic.
		if (prevT !== null && t + dayOffset < prevT - 12 * 3600 * 1000)
			dayOffset += 24 * 3600 * 1000;

		t += dayOffset;
		prevT = t;
		if (first === null)
			first = t;

		last = t;

		const a = APPLY.exec(line);
		if (a)
		{
			applies.push({ t, kind: a[2] ? "applied" : "cleared", preset: a[2] || "", camera: Number(a[3] ?? a[4]) });
			continue;
		}

		for (const [kind, rx] of EVENTS)
		{
			if (rx.test(line))
			{
				events.push({ t, kind });
				break;
			}
		}
	}

	// Attribute each apply: a changed camera index (read from the apply lines) first, else the closest preceding
	// event within the window.
	const byCause = {};
	let prevCamera = null;
	let ei = -1;
	for (const a of applies)
	{
		while (ei + 1 < events.length && events[ei + 1].t <= a.t)
			ei++;

		let cause = "UNATTRIBUTED";
		if (prevCamera !== null && a.camera !== prevCamera)
			cause = "CAMERA_INDEX";
		else if (ei >= 0 && a.t - events[ei].t <= windowMs)
			cause = events[ei].kind;

		prevCamera = a.camera;
		a.cause = cause;
		byCause[cause] = (byCause[cause] || 0) + 1;
	}

	// Bursts: applies less than 500 ms after the previous one.
	let burst = 0;
	for (let i = 1; i < applies.length; i++)
		if (applies[i].t - applies[i - 1].t < 500)
			burst++;

	const minutes = first !== null ? (last - first) / 60000 : 0;
	const userApplies = byCause.USER || 0;
	return {
		minutes,
		applied: applies.filter((a) => a.kind === "applied").length,
		cleared: applies.filter((a) => a.kind === "cleared").length,
		perHour: minutes > 0 ? (applies.length * 60) / minutes : NaN,
		perHourNonUser: minutes > 0 ? ((applies.length - userApplies) * 60) / minutes : NaN,
		burstApplies: burst,
		cameras: [...new Set(applies.map((a) => a.camera))],
		presets: [...new Set(applies.filter((a) => a.preset).map((a) => a.preset))],
		byCause,
	};
}

if (process.argv[1] && fileURLToPath(import.meta.url) === process.argv[1])
{
	const args = process.argv.slice(2);
	const json = args.includes("--json");
	const wi = args.indexOf("--window");
	const windowMs = wi >= 0 ? Number(args[wi + 1]) : 2000;
	const files = args.filter((a, i) => a !== "--json" && a !== "--window" && (wi < 0 || i !== wi + 1));
	if (files.length === 0 || !(windowMs > 0))
	{
		console.error("Usage: node tools/nml-visuals/v1-log-analysis.mjs <console.log> [...] [--window <ms>] [--json]");
		process.exit(2);
	}

	const results = files.map((f) => ({ file: f, ...analyze(fs.readFileSync(f, "utf8"), windowMs) }));
	if (json)
	{
		console.log(JSON.stringify(results, null, 2));
	}
	else
	{
		for (const r of results)
		{
			const causes = Object.entries(r.byCause).sort((a, b) => b[1] - a[1]).map(([k, n]) => `${k} ${n}`).join(", ");
			console.log(`${path.basename(path.dirname(r.file))}: ${r.minutes.toFixed(1)} min, applied ${r.applied}, cleared ${r.cleared}, ` +
				`${r.perHour.toFixed(1)}/h (${r.perHourNonUser.toFixed(1)}/h excluding user changes), bursts ${r.burstApplies}, cameras [${r.cameras}], presets [${r.presets}]`);
			console.log(`  nearest preceding event within ${windowMs} ms (inferred, not proven): ${causes || "none"}`);
		}
	}
}
