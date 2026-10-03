// Tests for tools/modlist.mjs: each case builds a tiny fixture repo in the OS temp dir.
// Usage: node tools/tests/modlist.test.mjs

import { spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

const MODLIST = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..", "modlist.mjs");

const BASE = "58D0FB3206B6F859";
const T = "1A1A1A1A1A1A1A1A";
const D = "2B2B2B2B2B2B2B2B";
const E = "3C3C3C3C3C3C3C3C";
const gproj = (id, guid, deps) =>
	`GameProject {\n ID "${id}"\n GUID "${guid}"\n TITLE "${id}"\n Dependencies {\n${deps.map((d) => `  "${d}"`).join("\n")}\n }\n}\n`;
const entry = (id, name, extra = {}) => ({ name, modId: id, version: "1.0.0", ...extra });
const registry = (mods) => JSON.stringify({ schemaVersion: 1, mods });
const PAIR = [entry(T, "Terrain", { version: "1.0.5", requires: [D] }), entry(D, "Dep", { version: "2.0.4", requiredBy: [T] })];

function baseTree(mods = PAIR)
{
	return {
		"dependencies/mods.json": registry(mods),
		"server/configs/test.template.json": "{\n\t\"game\": { \"mods\": [] }\n}\n",
		"addons/NML_Core/NML_Core.gproj": gproj("NML_Core", "1111111111111111", [BASE]),
		"addons/NML_Scenario_X/NML_Scenario_X.gproj": gproj("NML_Scenario_X", "4444444444444444", [BASE, "1111111111111111", T]),
		"addons/NML_Scenario_Plain/NML_Scenario_Plain.gproj": gproj("NML_Scenario_Plain", "5555555555555555", [BASE, "1111111111111111"]),
	};
}

function run(tree, args)
{
	const dir = fs.mkdtempSync(path.join(os.tmpdir(), "nml-modlist-"));
	try
	{
		for (const [p, c] of Object.entries(tree))
		{
			const f = path.join(dir, p);
			fs.mkdirSync(path.dirname(f), { recursive: true });
			fs.writeFileSync(f, c);
		}
		const r = spawnSync(process.execPath, [MODLIST, "--repo", dir, ...args], { encoding: "utf8" });
		const configAfter = fs.readFileSync(path.join(dir, "server/configs/test.template.json"), "utf8");
		return { code: r.status, out: r.stdout, err: r.stderr, configAfter, configBefore: tree["server/configs/test.template.json"] };
	}
	finally
	{
		fs.rmSync(dir, { recursive: true, force: true });
	}
}

let fail = 0;
let total = 0;
function check(name, ok, detail = "")
{
	total++;
	if (!ok)
		fail++;
	console.log(`${ok ? "PASS" : "FAIL"}  ${name}${ok ? "" : "\n      " + detail}`);
}

// dependencies come first, then dependents
let r = run(baseTree(), ["--root", T, "--format", "guids"]);
check("guids format lists dependencies before dependents", r.code === 0 && r.out.trim() === `${D},${T}`, r.out + r.err);

r = run(baseTree(), ["--root", T, "--format", "json"]);
const parsed = r.code === 0 ? JSON.parse(r.out) : null;
check("json format is a pinned {modId,name,version} array", parsed && parsed.length === 2 && parsed[0].modId === D && parsed[0].version === "2.0.4" && parsed[1].name === "Terrain", r.out + r.err);

r = run(baseTree(), ["--root", T]);
check("default format is a pasteable \"mods\" block", r.code === 0 && r.out.startsWith("\"mods\": [") && r.out.includes(`"version": "1.0.5"`), r.out + r.err);

const a = run(baseTree(), ["--root", T, "--format", "json"]);
const b = run(baseTree([...PAIR].reverse()), ["--root", T, "--format", "json"]);
check("output is deterministic regardless of registry order", a.code === 0 && a.out === b.out, a.out + "\n---\n" + b.out);

r = run(baseTree(), ["--scenario", "NML_Scenario_X", "--format", "guids"]);
check("--scenario resolves roots through the gproj (via NML addons)", r.code === 0 && r.out.trim() === `${D},${T}`, r.out + r.err);

r = run(baseTree(), ["--scenario", "X", "--format", "guids"]);
check("--scenario accepts the short name", r.code === 0 && r.out.trim() === `${D},${T}`, r.out + r.err);

r = run(baseTree(), ["--root", T, "--format", "guids"]);
check("stdout only: server/configs is not written", r.configAfter === r.configBefore, r.configAfter);

r = run(baseTree(), ["--root", "9999999999999999"]);
check("unregistered root fails", r.code === 1 && /not registered in dependencies\/mods\.json/.test(r.err), r.err);

r = run(baseTree([entry(T, "Terrain", { requires: [E] })]), ["--root", T]);
check("incomplete closure fails clearly", r.code === 1 && /incomplete closure.*requires 3C3C3C3C3C3C3C3C, which is not registered/.test(r.err) && r.out === "", r.err);

r = run(baseTree([entry(T, "Terrain", { requires: [D] }), entry(D, "Dep", { requires: [T] })]), ["--root", T]);
check("dependency cycle fails", r.code === 1 && /dependency cycle/.test(r.err), r.err);

r = run(baseTree([entry(T, "Terrain", { version: "" })]), ["--root", T]);
check("missing pinned version fails", r.code === 1 && /no pinned version/.test(r.err), r.err);

r = run(baseTree(), ["--scenario", "Plain"]);
check("scenario without third-party dependencies fails", r.code === 1 && /no third-party roots/.test(r.err), r.err);

r = run(baseTree(), ["--scenario", "Nope"]);
check("unknown scenario fails", r.code === 1 && /no addon "Nope"/.test(r.err), r.err);

r = run(baseTree(), ["--root", T, "--format", "xml"]);
check("bad --format fails", r.code === 1 && /--format must be/.test(r.err), r.err);

r = run(baseTree(), []);
check("no selector fails", r.code === 1 && /give --root/.test(r.err), r.err);

console.log(`\n${total - fail}/${total} passed`);
process.exitCode = fail ? 1 : 0;
