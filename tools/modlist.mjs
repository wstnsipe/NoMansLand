// Print a pinned game.mods list from the dependency registry. Zero dependencies, Node >= 18.
//
// Usage: node tools/modlist.mjs (--root <GUID[,GUID...]> | --scenario <addon>) [--format mods|json|guids] [--repo <dir>]
//   --root      registry mod(s) to start from; the output is their complete dependency closure
//   --scenario  an addon under addons/ (e.g. NML_Scenario_Myrove, or just Myrove); its non-NML
//               .gproj dependencies are the roots (NML addons are followed, base game is skipped)
//   --format    mods (default): a pasteable `"mods": [...]` block; json: the bare array; guids: comma list for -addons
//   --repo      repository root (default: this checkout; used by the tests)
//
// Output goes to stdout only; nothing is written to server/configs. Dependencies come before
// dependents, ties broken by modId, so the same registry always gives the same output.
// Exit code 1, with a message on stderr, if a root is not registered, a required mod is not
// registered, the closure has a cycle, or an entry has no version.

import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";
import { GUID_RE, closure, parseIds } from "./lib/registry-closure.mjs";

const BASE_GAME_GUID = "58D0FB3206B6F859";

function fail(msg)
{
	console.error(`modlist: ${msg}`);
	process.exit(1);
}

const argv = process.argv.slice(2);
const opt = (name) => (argv.includes(name) ? argv[argv.indexOf(name) + 1] : undefined);
const ROOT = path.resolve(opt("--repo") ?? path.join(path.dirname(fileURLToPath(import.meta.url)), ".."));
const format = opt("--format") ?? "mods";
if (!["mods", "json", "guids"].includes(format))
	fail(`--format must be mods|json|guids, got "${format}"`);

const registryFile = path.join(ROOT, "dependencies", "mods.json");
if (!fs.existsSync(registryFile))
	fail("dependencies/mods.json is missing");
let reg;
try
{
	reg = JSON.parse(fs.readFileSync(registryFile, "utf8"));
}
catch (e)
{
	fail(`dependencies/mods.json is not valid JSON: ${e.message}`);
}
const registry = new Map((reg.mods || []).map((m) => [String(m.modId).toUpperCase(), m]));

// NML addons: guid -> { name, deps }
function readAddons()
{
	const out = new Map();
	const dir = path.join(ROOT, "addons");
	if (!fs.existsSync(dir))
		return out;
	for (const e of fs.readdirSync(dir, { withFileTypes: true }).filter((d) => d.isDirectory()))
	{
		const gproj = path.join(dir, e.name, `${e.name}.gproj`);
		if (!fs.existsSync(gproj))
			continue;
		const text = fs.readFileSync(gproj, "utf8");
		const guid = /\bGUID\s+"([^"]+)"/.exec(text)?.[1]?.toUpperCase();
		const block = /Dependencies\s*\{([^}]*)\}/.exec(text)?.[1] ?? "";
		const deps = [...block.matchAll(/"([0-9A-Fa-f]{16})"/g)].map((m) => m[1].toUpperCase());
		if (guid)
			out.set(guid, { name: e.name, deps });
	}
	return out;
}

function scenarioRoots(name)
{
	const addons = readAddons();
	const found = [...addons.entries()].find(([, a]) => a.name === name || a.name === `NML_Scenario_${name}`);
	if (!found)
		fail(`no addon "${name}" (or NML_Scenario_${name}) under addons/`);
	const roots = new Set();
	const seen = new Set();
	const walk = (guid) =>
	{
		if (seen.has(guid))
			return;
		seen.add(guid);
		for (const dep of addons.get(guid).deps)
		{
			if (dep === BASE_GAME_GUID)
				continue;
			if (addons.has(dep))
				walk(dep);
			else
				roots.add(dep);
		}
	};
	walk(found[0]);
	return [...roots];
}

let roots;
if (opt("--root"))
	roots = parseIds(opt("--root"));
else if (opt("--scenario"))
	roots = scenarioRoots(opt("--scenario"));
else
	fail("give --root <GUID[,GUID...]> or --scenario <addon>");
if (roots.length === 0)
	fail("no third-party roots: the selection has no registry dependencies");
for (const r of roots)
{
	if (!GUID_RE.test(r))
		fail(`root "${r}" is not a 16-hex GUID`);
	if (!registry.has(r))
		fail(`root ${r} is not registered in dependencies/mods.json`);
}

const { order, missing, cycles } = closure(registry, roots);
if (missing.length)
	fail(`incomplete closure: ${missing.map((m) => `${m.by} (${registry.get(m.by).name}) requires ${m.id}, which is not registered`).join("; ")}`);
if (cycles.length)
	fail(`dependency cycle: ${cycles.map((c) => c.join(" -> ")).join("; ")}`);
const noVersion = order.filter((id) => !registry.get(id).version);
if (noVersion.length)
	fail(`no pinned version in the registry for: ${noVersion.join(", ")}`);

const mods = order.map((id) => ({ modId: id, name: registry.get(id).name, version: registry.get(id).version }));
if (format === "guids")
	console.log(order.join(","));
else if (format === "json")
	console.log(JSON.stringify(mods, null, "\t"));
else
	console.log(`"mods": ${JSON.stringify(mods, null, "\t")}`);
