// Scan Arma Reforger / Workbench logs for the problems a compatibility run cares about.
// Zero dependencies, Node >= 18.
//
// Usage: node tools/log-scan.mjs <log-file-or-dir>... [--json] [--baseline <earlier --json output>] [--fail-on <cat,cat>]
//   A directory is searched for console.log and error.log files.
//   --json       machine-readable output (also usable as a baseline later)
//   --baseline   findings already present in that earlier run are reported as "known", not new
//   --fail-on    exit 1 on new findings in these categories only (default: every error category)
//
// Each (E)-level line is matched against RULES in order; the first match decides the category.
// A line that matches no rule but is an (E) line is counted as "other-error" (reported, not failing
// unless named in --fail-on). Patterns marked evidence "local-log" were copied from real logs;
// "documented" come from this repo's docs; "unverified" are expected formats not yet seen in a log.
//
// Exit code: 0 clean, 1 new failing findings, 2 usage error.

import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

export const RULES = [
	{ category: "missing-addon", severity: "error", evidence: "local-log", re: /Failed to fetch addon details from workshop API/ },
	{ category: "missing-addon", severity: "error", evidence: "local-log", re: /Attempt to download an empty package/ },
	{ category: "missing-addon", severity: "error", evidence: "documented", re: /Addon was not found on workshop/ },
	{ category: "wrong-guid", severity: "error", evidence: "local-log", re: /Wrong GUID\/name for resource @"\{[0-9A-F]{16}\}/ },
	{ category: "wrong-guid", severity: "error", evidence: "local-log", re: /Broken resource GUID=[0-9A-F]{16}!/ },
	{ category: "platform-init", severity: "error", evidence: "local-log", re: /Unable to initialize the game/ },
	{ category: "platform-init", severity: "error", evidence: "local-log", re: /SteamAPI_Init failed/ },
	{ category: "script-compile", severity: "error", evidence: "unverified", re: /Can't compile "?\w+"? script module/ },
	{ category: "script-compile", severity: "error", evidence: "unverified", re: /SCRIPT\s+\(E\):\s*@"[^"]+,\d+(?:,\d+)?"/ },
	{ category: "rpc", severity: "error", evidence: "unverified", re: /\(E\):.*\b(?:Rpc|RPC|RplRpc)\w*/ },
	{ category: "replication", severity: "error", evidence: "local-log", re: /\bRPL\s+\(E\):/ },
	{ category: "script-error", severity: "error", evidence: "local-log", re: /\bSCRIPT\s+\(E\):/ },
];

const E_LINE = /\(E\):/;
const TIMESTAMP = /^\s*\d{1,2}:\d{2}:\d{2}\.\d{3}\s+/;

// Returns [{ category, severity, key, text, line }] for one log's text. `key` is the line without
// its timestamp, so the same message at different times collapses into one finding.
export function scanText(text)
{
	const out = [];
	const lines = text.split(/\r?\n/);
	for (let i = 0; i < lines.length; i++)
	{
		const raw = lines[i];
		const hit = RULES.find((r) => r.re.test(raw));
		if (!hit && !E_LINE.test(raw))
			continue;
		const message = raw.replace(TIMESTAMP, "").replace(/\s+/g, " ").trim();
		const category = hit ? hit.category : "other-error";
		out.push({ category, severity: hit ? hit.severity : "info", key: `${category}|${message}`, text: message, line: i + 1 });
	}
	return out;
}

function collectFiles(target)
{
	const st = fs.statSync(target);
	if (st.isFile())
		return [target];
	const out = [];
	for (const e of fs.readdirSync(target, { withFileTypes: true }))
	{
		const p = path.join(target, e.name);
		if (e.isDirectory())
			out.push(...collectFiles(p));
		else if (e.name === "console.log" || e.name === "error.log")
			out.push(p);
	}
	return out;
}

// Merge findings across files into one entry per key, counting occurrences.
export function summarize(perFile, baselineKeys = new Set())
{
	const byKey = new Map();
	for (const { file, findings } of perFile)
	{
		for (const f of findings)
		{
			const cur = byKey.get(f.key);
			if (cur)
				cur.count++;
			else
				byKey.set(f.key, { category: f.category, severity: f.severity, key: f.key, text: f.text, count: 1, first: { file, line: f.line }, known: baselineKeys.has(f.key) });
		}
	}
	return [...byKey.values()].sort((a, b) => a.category.localeCompare(b.category) || a.text.localeCompare(b.text));
}

function main()
{
	const argv = process.argv.slice(2);
	const opt = (name) => (argv.includes(name) ? argv[argv.indexOf(name) + 1] : undefined);
	const valueArgs = new Set([opt("--baseline"), opt("--fail-on")]);
	const targets = argv.filter((a) => !a.startsWith("--") && !valueArgs.has(a));
	if (targets.length === 0)
	{
		console.error("log-scan: give at least one log file or directory");
		process.exit(2);
	}
	const files = [];
	for (const t of targets)
	{
		if (!fs.existsSync(t))
		{
			console.error(`log-scan: not found: ${t}`);
			process.exit(2);
		}
		files.push(...collectFiles(t));
	}
	if (files.length === 0)
	{
		console.error("log-scan: no console.log or error.log files found");
		process.exit(2);
	}
	let baselineKeys = new Set();
	if (opt("--baseline"))
	{
		try
		{
			baselineKeys = new Set(JSON.parse(fs.readFileSync(opt("--baseline"), "utf8")).findings.map((f) => f.key));
		}
		catch (e)
		{
			console.error(`log-scan: cannot read baseline: ${e.message}`);
			process.exit(2);
		}
	}
	const failOn = opt("--fail-on") ? new Set(opt("--fail-on").split(",")) : null;
	const perFile = files.map((file) => ({ file, findings: scanText(fs.readFileSync(file, "utf8")) }));
	const findings = summarize(perFile, baselineKeys);
	const failing = findings.filter((f) => !f.known && (failOn ? failOn.has(f.category) : f.severity === "error"));

	if (argv.includes("--json"))
	{
		console.log(JSON.stringify({ files, findings, failing: failing.length }, null, "\t"));
	}
	else
	{
		const cats = [...new Set(findings.map((f) => f.category))];
		console.log(`log-scan: ${files.length} file(s), ${findings.length} distinct finding(s), ${failing.length} new failing`);
		for (const c of cats)
		{
			const inCat = findings.filter((f) => f.category === c);
			console.log(`\n${c}: ${inCat.length} distinct, ${inCat.filter((f) => !f.known).length} new`);
			for (const f of inCat.filter((x) => !x.known).slice(0, 10))
				console.log(`  x${f.count}  ${f.text.slice(0, 200)}  (${path.basename(path.dirname(f.first.file))}/${path.basename(f.first.file)}:${f.first.line})`);
		}
	}
	process.exitCode = failing.length ? 1 : 0;
}

if (process.argv[1] && path.resolve(process.argv[1]) === fileURLToPath(import.meta.url))
	main();
