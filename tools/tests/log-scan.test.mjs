// Tests for tools/log-scan.mjs. Fixture lines marked "real" were copied from Reforger logs
// (Stage 6 spike and probe runs); lines marked "synthetic" are expected formats not yet seen.
// Usage: node tools/tests/log-scan.test.mjs

import { spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { RULES, scanText, summarize } from "../log-scan.mjs";

const SCAN = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..", "log-scan.mjs");

const FIXTURES = [
	// [category, line, source]
	["wrong-guid", "22:06:39.290  RESOURCES (E): Wrong GUID/name for resource @\"{BF74128ABE05E424}MI_COL.gamemat\" in property \"UBX_boxes_dirty\"", "real"],
	["wrong-guid", "22:07:21.956 RESOURCES (E): Broken resource GUID=0000000000000000!", "real"],
	["missing-addon", "12:43:45.312 BACKEND   (E): Failed to fetch addon details from workshop API! Repeat later or try different mods.", "real"],
	["missing-addon", "12:43:45.308 DEFAULT   (E): Attempt to download an empty package!", "real"],
	["missing-addon", "12:00:00.000 BACKEND   (E): Addon was not found on workshop", "documented"],
	["platform-init", "12:43:45.399 ENGINE    (E): Unable to initialize the game", "real"],
	["platform-init", "Steamworks: SteamAPI_Init failed. Is Steam running?", "real"],
	["script-compile", "12:00:01.000 SCRIPT    (E): @\"Scripts/Game/NML/Arsenal/NML_Test.c,12\": Unknown type 'NML_Nope'", "synthetic"],
	["script-compile", "12:00:01.000 SCRIPT    (E): Can't compile \"Game\" script module!", "synthetic"],
	["rpc", "12:00:02.000 NETWORK   (E): Rpc call failed: RpcAsk_Something not found on target", "synthetic"],
	["replication", "22:06:40.504      RPL       (E): RPL state override set to Runtime on an entity instance without prefab. Either remove RPL state override setting, or replace entity instance with prefab.", "real"],
	["script-error", "12:00:03.000 SCRIPT    (E): Can't instantiate class 'SCR_FilterCategory', constructor is not public", "real"],
	["other-error", "12:00:04.000 AI        (E): No AIWorld is present in the map. Move handlers won't be created.", "real"],
];
// Lines that must never be reported.
const CLEAN = [
	"12:00:05.000 SCRIPT    (W): @\"scripts/Game/Workshop/SCR_AddonManager.c,876\": 'GetRefCount' is obsolete: Will be removed",
	"12:00:05.000 BACKEND      : Downloading 65A2EA40DC9E632A version 1.0.12",
	"12:00:05.000 ENGINE       : Game successfully created.",
	"12:00:05.000 BACKEND      : Required addons are ready to use.",
];

let fail = 0;
let total = 0;
function check(name, ok, detail = "")
{
	total++;
	if (!ok)
		fail++;
	console.log(`${ok ? "PASS" : "FAIL"}  ${name}${ok ? "" : "\n      " + detail}`);
}

for (const [category, line, source] of FIXTURES)
{
	const found = scanText(line);
	check(`[${source}] ${category}: ${line.replace(/^[\d:.]+\s+/, "").slice(0, 60)}`, found.length === 1 && found[0].category === category, JSON.stringify(found));
}
for (const line of CLEAN)
	check(`clean line ignored: ${line.replace(/^[\d:.]+\s+/, "").slice(0, 60)}`, scanText(line).length === 0, JSON.stringify(scanText(line)));

check("every rule category has a fixture", [...new Set(RULES.map((r) => r.category))].every((c) => FIXTURES.some((f) => f[0] === c)), "rule categories without fixture");
check("every rule has an evidence label", RULES.every((r) => ["local-log", "documented", "unverified"].includes(r.evidence)), "missing evidence label");

// Same message at different times collapses into one finding.
const twice = scanText("10:00:00.000 RESOURCES (E): Broken resource GUID=0000000000000000!\n11:11:11.111 RESOURCES (E): Broken resource GUID=0000000000000000!");
check("same message at different times has one key", twice.length === 2 && twice[0].key === twice[1].key, JSON.stringify(twice));
const merged = summarize([{ file: "a.log", findings: twice }]);
check("summarize counts occurrences", merged.length === 1 && merged[0].count === 2, JSON.stringify(merged));

// CLI: exit codes, --json, --baseline, --fail-on.
const dir = fs.mkdtempSync(path.join(os.tmpdir(), "nml-logscan-"));
try
{
	const logs = path.join(dir, "logs_1");
	fs.mkdirSync(logs);
	fs.writeFileSync(path.join(logs, "console.log"), FIXTURES.map((f) => f[1]).join("\n") + "\n" + CLEAN.join("\n") + "\n");
	fs.writeFileSync(path.join(dir, "clean.log"), CLEAN.join("\n") + "\n");
	const cli = (...a) => spawnSync(process.execPath, [SCAN, ...a], { encoding: "utf8" });

	let r = cli(dir);
	check("CLI finds console.log in a directory and exits 1 on errors", r.status === 1 && /wrong-guid: 2 distinct/.test(r.stdout), r.stdout + r.stderr);

	r = cli(path.join(dir, "clean.log"));
	check("CLI exits 0 on a clean log", r.status === 0 && /0 new failing/.test(r.stdout), r.stdout + r.stderr);

	r = cli(dir, "--json");
	const json = JSON.parse(r.stdout);
	check("--json lists findings with category and count", json.findings.length === FIXTURES.length && json.findings.every((f) => f.category && f.count >= 1), r.stdout.slice(0, 300));
	fs.writeFileSync(path.join(dir, "baseline.json"), r.stdout);

	r = cli(dir, "--baseline", path.join(dir, "baseline.json"));
	check("--baseline marks earlier findings known and exits 0", r.status === 0 && /0 new failing/.test(r.stdout), r.stdout + r.stderr);

	fs.writeFileSync(path.join(logs, "console.log"), "12:00:09.000 RESOURCES (E): Broken resource GUID=1111111111111111!\n");
	r = cli(dir, "--baseline", path.join(dir, "baseline.json"));
	check("--baseline still fails on a new finding", r.status === 1 && /1 new failing/.test(r.stdout), r.stdout + r.stderr);

	fs.writeFileSync(path.join(logs, "console.log"), "12:00:04.000 AI        (E): No AIWorld is present in the map. Move handlers won't be created.\n");
	r = cli(dir);
	check("other-error alone does not fail by default", r.status === 0, r.stdout + r.stderr);
	r = cli(dir, "--fail-on", "other-error");
	check("--fail-on other-error makes it fail", r.status === 1, r.stdout + r.stderr);

	r = cli();
	check("no arguments is a usage error (exit 2)", r.status === 2, r.stdout + r.stderr);
	r = cli(path.join(dir, "does-not-exist"));
	check("missing path is a usage error (exit 2)", r.status === 2 && /not found/.test(r.stderr), r.stdout + r.stderr);
}
finally
{
	fs.rmSync(dir, { recursive: true, force: true });
}

console.log(`\n${total - fail}/${total} passed`);
process.exitCode = fail ? 1 : 0;
