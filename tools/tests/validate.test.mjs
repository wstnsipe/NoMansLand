// Tests for tools/validate.mjs: each case builds a tiny fixture repo in the OS
// temp dir and checks that the validator passes or fails with the expected error.
// Usage: node tools/tests/validate.test.mjs

import { spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

const VALIDATE = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..", "validate.mjs");

const BASE = "58D0FB3206B6F859";
const gproj = (id, guid, deps = [BASE]) =>
	`GameProject {\n ID "${id}"\n GUID "${guid}"\n TITLE "${id}"\n Dependencies {\n${deps.map((d) => `  "${d}"`).join("\n")}\n }\n}\n`;
const registryEmpty = JSON.stringify({ schemaVersion: 1, mods: [] });

// Registry fixtures: T (terrain) requires D (dependency); E is never registered.
const T = "1A1A1A1A1A1A1A1A";
const D = "2B2B2B2B2B2B2B2B";
const E = "3C3C3C3C3C3C3C3C";
const approved = (name) => `---\nname: ${name}\nstatus: approved\ncategory: content\nproposedBy: m\nproposedOn: 2026-09-30\napprovedBy: wstnsipe\n---\n`;
const entry = (id, name, extra = {}) => ({
	name, modId: id, version: "1.0.0", purpose: "p", required: true, scope: "scenario", channel: "stable", licenseClass: "APL", sizeMB: 1,
	candidate: `candidates/${name}.md`, approvedBy: "wstnsipe", approvedOn: "2026-10-01", ...extra,
});
// Tree overrides that register `mods` (each needs an approved candidate file).
const registerMods = (mods) => ({
	"dependencies/mods.json": JSON.stringify({ schemaVersion: 1, mods }),
	...Object.fromEntries(mods.map((m) => [`dependencies/${m.candidate}`, approved(m.name)])),
});
const goodPair = [entry(T, "Terrain", { requires: [D] }), entry(D, "Dep", { requiredBy: [T] })];
const testConfig = (...ids) => JSON.stringify({ game: { mods: ids.map((id) => ({ modId: id, name: id, version: "1.0.0" })) } });

// Minimal valid tree; each case overrides/adds files.
function baseTree()
{
	return {
		"dependencies/mods.json": registryEmpty,
		"addons/NML_Core/NML_Core.gproj": gproj("NML_Core", "1111111111111111"),
		"addons/NML_Core/Scripts/Game/NML/Loot/NML_LootComponent.c": "class NML_LootComponentClass : ScriptComponentClass\n{\n}\n\nclass NML_LootComponent : ScriptComponent\n{\n}\n",
		"addons/NML_Core/Configs/NML/Loot.conf": "SCR_Something {\n}\n",
		"addons/NML_Core/Configs/NML/Loot.conf.meta": "MetaFileClass {\n Name \"{AAAAAAAAAAAAAAAA}Configs/NML/Loot.conf\"\n}\n",
		"server/configs/dev.json": JSON.stringify({ game: { passwordAdmin: "", mods: [{ modId: "1111111111111111", name: "NML_Core" }] } }),
		"server/configs/live.template.json": JSON.stringify({ game: { passwordAdmin: "${NML_ADMIN_PASSWORD}", admins: ["${NML_ADMIN_1}"], mods: [{ modId: "1111111111111111", name: "NML_Core", version: "0.1.0" }] } }),
	};
}

function run(tree, extraArgs = [])
{
	const dir = fs.mkdtempSync(path.join(os.tmpdir(), "nml-validate-"));
	try
	{
		for (const [p, c] of Object.entries(tree))
		{
			if (c === null)
				continue;
			const f = path.join(dir, p);
			fs.mkdirSync(path.dirname(f), { recursive: true });
			fs.writeFileSync(f, c);
		}
		const r = spawnSync(process.execPath, [VALIDATE, "--root", dir, ...extraArgs], { encoding: "utf8" });
		return { code: r.status, out: r.stdout + r.stderr };
	}
	finally
	{
		fs.rmSync(dir, { recursive: true, force: true });
	}
}

const cases = [
	["valid minimal tree passes", {}, 0, null],
	["missing .meta fails", { "addons/NML_Core/Configs/NML/Loot.conf.meta": null }, 1, /resource has no \.meta/],
	["orphan .meta fails", { "addons/NML_Core/Configs/NML/Gone.conf.meta": "MetaFileClass {\n Name \"{BBBBBBBBBBBBBBBB}x\"\n}\n" }, 1, /orphan \.meta/],
	["duplicate resource GUID fails", {
		"addons/NML_Core/Configs/NML/Two.conf": "X {\n}\n",
		"addons/NML_Core/Configs/NML/Two.conf.meta": "MetaFileClass {\n Name \"{AAAAAAAAAAAAAAAA}Configs/NML/Two.conf\"\n}\n",
	}, 1, /duplicate resource GUID/],
	["untagged class fails", { "addons/NML_Core/Scripts/Game/NML/Loot/NML_Bad.c": "class LootThing\n{\n}\n" }, 1, /class LootThing must start with NML_/],
	["untagged enum fails", { "addons/NML_Core/Scripts/Game/NML/Loot/NML_Enums.c": "enum NML_LootType\n{\n}\n" }, 1, /enum NML_LootType must start with NML_E/],
	["untagged file name fails", { "addons/NML_Core/Scripts/Game/NML/Loot/Loot.c": "class NML_X\n{\n}\n" }, 1, /file names must start with NML_/],
	["modded class outside Modded/ fails", { "addons/NML_Core/Scripts/Game/NML/Loot/NML_Mod.c": "modded class SCR_BaseGameMode\n{\n}\n" }, 1, /must live under a Modded\/ folder/],
	["modded class inside Modded/ passes", { "addons/NML_Core/Scripts/Game/NML/Modded/SCR_BaseGameMode.c": "modded class SCR_BaseGameMode\n{\n\tvoid NML_Hook()\n\t{\n\t}\n}\n" }, 0, null],
	["class in comment ignored", { "addons/NML_Core/Scripts/Game/NML/Loot/NML_Doc.c": "// class NotReal\n/* enum AlsoNotReal */\nclass NML_Real\n{\n}\n" }, 0, null],
	["EnfusionMCP handlers fail", { "addons/NML_Core/Scripts/WorkbenchGame/EnfusionMCP/EMCP_WB_Ping.c": "class EMCP_WB_Ping\n{\n}\n" }, 1, /handler scripts must never be committed/],
	["unregistered gproj dependency fails", { "addons/NML_Core/NML_Core.gproj": gproj("NML_Core", "1111111111111111", [BASE, "ABCDEF0123456789"]) }, 1, /not an approved entry/],
	["NML-to-NML dependency passes", {
		"addons/NML_Content/NML_Content.gproj": gproj("NML_Content", "2222222222222222", [BASE, "1111111111111111"]),
	}, 0, null],
	["missing base game dependency fails", { "addons/NML_Core/NML_Core.gproj": gproj("NML_Core", "1111111111111111", []) }, 1, /must depend on the base game/],
	["gproj ID mismatch fails", { "addons/NML_Core/NML_Core.gproj": gproj("Core", "1111111111111111") }, 1, /must equal the folder name/],
	["untagged addon folder fails", { "addons/Core/Core.gproj": gproj("Core", "3333333333333333") }, 1, /must start with NML_/],
	["live config without version fails", {
		"server/configs/live.template.json": JSON.stringify({ game: { mods: [{ modId: "1111111111111111", name: "NML_Core" }] } }),
	}, 1, /must pin "version"/],
	["literal secret in config fails", {
		"server/configs/dev.json": JSON.stringify({ rcon: { password: "hunter2" }, game: { mods: [] } }),
	}, 1, /literal secret/],
	["literal admin id fails", {
		"server/configs/live.template.json": JSON.stringify({ game: { admins: ["76561198000000000"], mods: [] } }),
	}, 1, /admins must only contain/],
	["unknown mod in server config fails", {
		"server/configs/dev.json": JSON.stringify({ game: { mods: [{ modId: "ABCDEF0123456789", name: "Random" }] } }),
	}, 1, /neither an NML addon nor in dependencies/],
	["registry entry without approved candidate fails", {
		"dependencies/mods.json": JSON.stringify({ schemaVersion: 1, mods: [{ name: "X", modId: "ABCDEF0123456789", version: "1.0.0", purpose: "p", required: true, scope: "content", channel: "stable", licenseClass: "APL", sizeMB: 1, candidate: "candidates/x.md", approvedBy: "wstnsipe", approvedOn: "2026-10-01" }] }),
		"dependencies/candidates/x.md": "---\nname: X\nstatus: proposed\ncategory: content\nproposedBy: member\nproposedOn: 2026-09-30\n---\n",
	}, 1, /must have status approved/],
	["dev-channel mod on LIVE fails", {
		"dependencies/mods.json": JSON.stringify({ schemaVersion: 1, mods: [{ name: "X", modId: "ABCDEF0123456789", version: "1.0.0", purpose: "p", required: true, scope: "content", channel: "dev", licenseClass: "GPL", sizeMB: 1, candidate: "candidates/x.md", approvedBy: "wstnsipe", approvedOn: "2026-10-01" }] }),
		"dependencies/candidates/x.md": "---\nname: X\nstatus: approved\ncategory: content\nproposedBy: member\nproposedOn: 2026-09-30\napprovedBy: wstnsipe\n---\n",
		"server/configs/live.template.json": JSON.stringify({ game: { mods: [{ modId: "ABCDEF0123456789", name: "X", version: "1.0.0" }] } }),
	}, 1, /Dev-channel build/],
	["bad candidate status fails", { "dependencies/candidates/y.md": "---\nname: Y\nstatus: maybe\ncategory: content\nproposedBy: m\nproposedOn: 2026-09-30\n---\n" }, 1, /status "maybe"/],
	["non-LFS binary fails in --ci", {
		"addons/NML_Content/NML_Content.gproj": gproj("NML_Content", "2222222222222222", [BASE, "1111111111111111"]),
		"addons/NML_Content/Assets/NML/tex.edds": "raw-binary",
		"addons/NML_Content/Assets/NML/tex.edds.meta": "MetaFileClass {\n Name \"{CCCCCCCCCCCCCCCC}Assets/NML/tex.edds\"\n}\n",
	}, 1, /not an LFS pointer/, ["--ci"]],
	["resourceDatabase.rdb needs no .meta", { "addons/NML_Core/resourceDatabase.rdb": "db" }, 0, null],
	["world .layer needs no .meta", {
		"addons/NML_Core/Worlds/NML/W/W.ent": "SubScene {\n}\n",
		"addons/NML_Core/Worlds/NML/W/W.ent.meta": "MetaFileClass {\n Name \"{DDDDDDDDDDDDDDDD}Worlds/NML/W/W.ent\"\n}\n",
		"addons/NML_Core/Worlds/NML/W/W_Layers/default.layer": "",
	}, 0, null],
	["world .ent without .meta still fails", {
		"addons/NML_Core/Worlds/NML/W/W.ent": "SubScene {\n}\n",
		"addons/NML_Core/Worlds/NML/W/W_Layers/default.layer": "",
	}, 1, /resource has no \.meta.*W\.ent/],

	// ---- Stage 6.3: licence class, registry closure, TEST/LIVE completeness, scenario closure
	["APL-SA licenseClass passes", registerMods([entry(D, "Dep", { licenseClass: "APL-SA" })]), 0, null],
	["unknown licenseClass fails", registerMods([entry(D, "Dep", { licenseClass: "APL-XX" })]), 1, /licenseClass must be GPL\|APL\|APL-SA\|APL-ND\|custom/],
	["valid requires/requiredBy closure passes", registerMods(goodPair), 0, null],
	["requires an unregistered mod fails", registerMods([entry(T, "Terrain", { requires: [E] })]), 1, /requires 3C3C3C3C3C3C3C3C, which is not registered/],
	["requires without matching requiredBy fails", registerMods([entry(T, "Terrain", { requires: [D] }), entry(D, "Dep")]), 1, /its requiredBy does not list 1A1A1A1A1A1A1A1A/],
	["requiredBy without matching requires fails", registerMods([entry(T, "Terrain"), entry(D, "Dep", { requiredBy: [T] })]), 1, /its requires does not list 2B2B2B2B2B2B2B2B/],
	["dependency cycle fails", registerMods([entry(T, "Terrain", { requires: [D], requiredBy: [D] }), entry(D, "Dep", { requires: [T], requiredBy: [T] })]), 1, /dependency cycle/],
	["malformed requires id fails", registerMods([entry(T, "Terrain", { requires: ["XYZ"] })]), 1, /requires entry "XYZ" must be 16 uppercase hex/],
	["complete TEST closure passes", { ...registerMods(goodPair), "server/configs/test.template.json": testConfig(T, D) }, 0, null],
	["incomplete TEST closure fails", { ...registerMods(goodPair), "server/configs/test.template.json": testConfig(T) }, 1, /test\.template\.json: mod 2B2B2B2B2B2B2B2B \(Dep\) is required by a listed mod but missing/],
	["incomplete LIVE closure fails", { ...registerMods(goodPair), "server/configs/live.template.json": testConfig(T) }, 1, /live\.template\.json: mod 2B2B2B2B2B2B2B2B \(Dep\) is required/],
	["unpinned dev config needs no closure", { ...registerMods(goodPair), "server/configs/dev.json": JSON.stringify({ game: { mods: [{ modId: T, name: "Terrain" }] } }) }, 0, null],
	["TEST list with a scenario addon but not its terrain fails", {
		...registerMods(goodPair),
		"addons/NML_Scenario_X/NML_Scenario_X.gproj": gproj("NML_Scenario_X", "4444444444444444", [BASE, "1111111111111111", T]),
		"server/configs/test.template.json": testConfig("1111111111111111", "4444444444444444"),
	}, 1, /test\.template\.json: mod 1A1A1A1A1A1A1A1A \(Terrain\) is required by a listed mod but missing/],
	["TEST list with a scenario addon and its full closure passes", {
		...registerMods(goodPair),
		"addons/NML_Scenario_X/NML_Scenario_X.gproj": gproj("NML_Scenario_X", "4444444444444444", [BASE, "1111111111111111", T]),
		"server/configs/test.template.json": testConfig("1111111111111111", "4444444444444444", T, D),
	}, 0, null],
	["scenario gproj with complete closure passes", {
		...registerMods(goodPair),
		"addons/NML_Scenario_X/NML_Scenario_X.gproj": gproj("NML_Scenario_X", "4444444444444444", [BASE, "1111111111111111", T]),
	}, 0, null],
	["scenario gproj with incomplete closure fails", {
		...registerMods([entry(T, "Terrain", { requires: [E] })]),
		"addons/NML_Scenario_X/NML_Scenario_X.gproj": gproj("NML_Scenario_X", "4444444444444444", [BASE, "1111111111111111", T]),
	}, 1, /NML_Scenario_X: dependency closure incomplete: 1A1A1A1A1A1A1A1A requires 3C3C3C3C3C3C3C3C/],
];

let fail = 0;
for (const [name, overrides, expectedCode, expectedOut, extra] of cases)
{
	const r = run({ ...baseTree(), ...overrides }, extra || []);
	const ok = r.code === expectedCode && (!expectedOut || expectedOut.test(r.out));
	if (!ok)
		fail++;
	console.log(`${ok ? "PASS" : "FAIL"}  ${name}  (exit ${r.code}, expected ${expectedCode})${ok ? "" : "\n" + r.out.split("\n").map((l) => "      " + l).join("\n")}`);
}
console.log(`\n${cases.length - fail}/${cases.length} passed`);
process.exitCode = fail ? 1 : 0;
