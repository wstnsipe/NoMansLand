// NML repository validator. Zero dependencies, Node >= 18.
// Used by CI (.github/workflows/ci.yml), the pre-commit hook, and developers.
//
// Usage: node tools/validate.mjs [--root <dir>] [--ci]
//   --root  validate another tree (used by tools/tests/validate.test.mjs)
//   --ci    also require LFS-tracked files to be LFS pointers (checkout without LFS smudge)
//
// Exit code 1 if any error is found. Warnings never fail the run.

import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const args = process.argv.slice(2);
const ROOT = path.resolve(args.includes("--root") ? args[args.indexOf("--root") + 1] : path.join(path.dirname(fileURLToPath(import.meta.url)), ".."));
const CI = args.includes("--ci");

const BASE_GAME_GUID = "58D0FB3206B6F859";
const TAG = "NML_";
const GUID_RE = /^[0-9A-F]{16}$/;
const SEMVER_RE = /^\d+\.\d+\.\d+$/;
const CANDIDATE_STATUSES = ["proposed", "under-review", "shortlisted", "approved", "registered", "rejected", "superseded", "native", "parked"];
const CATEGORIES = ["foundational", "content", "gameplay", "qol-admin", "alternative", "native"];
// Keep in sync with the LFS section of .gitattributes.
const LFS_EXT = [".xob", ".fbx", ".edds", ".dds", ".tif", ".tiff", ".tga", ".png", ".jpg", ".jpeg", ".psd", ".wav", ".ogg", ".anm"];
// Files that never get a .meta (scripts, project files, generated DBs, docs).
const NO_META_EXT = [".c", ".gproj", ".meta", ".rdb", ".md", ".txt"];

const errors = [];
const warnings = [];
const info = [];
const err = (area, msg) => errors.push(`[${area}] ${msg}`);
const warn = (area, msg) => warnings.push(`[${area}] ${msg}`);
const rel = (p) => path.relative(ROOT, p).replace(/\\/g, "/");

function walk(dir, out = [])
{
	if (!fs.existsSync(dir))
		return out;
	for (const e of fs.readdirSync(dir, { withFileTypes: true }))
	{
		const p = path.join(dir, e.name);
		if (e.isDirectory())
			walk(p, out);
		else
			out.push(p);
	}
	return out;
}

function readJson(file, area)
{
	try
	{
		return JSON.parse(fs.readFileSync(file, "utf8"));
	}
	catch (e)
	{
		err(area, `${rel(file)} is not valid JSON: ${e.message}`);
		return null;
	}
}

function frontmatter(file)
{
	const m = /^---\r?\n([\s\S]*?)\r?\n---/.exec(fs.readFileSync(file, "utf8"));
	if (!m)
		return null;
	const fm = {};
	for (const line of m[1].split(/\r?\n/))
	{
		const kv = /^([A-Za-z][\w-]*):\s*(.*)$/.exec(line);
		if (kv)
			fm[kv[1]] = kv[2].replace(/^["']|["']$/g, "").trim();
	}
	return fm;
}

// ---------------------------------------------------------------- registry
const registryFile = path.join(ROOT, "dependencies", "mods.json");
const registry = new Map();
function checkRegistry()
{
	if (!fs.existsSync(registryFile))
	{
		err("registry", "dependencies/mods.json is missing");
		return;
	}
	const reg = readJson(registryFile, "registry");
	if (!reg)
		return;
	if (reg.schemaVersion !== 1 || !Array.isArray(reg.mods))
	{
		err("registry", "mods.json must be { \"schemaVersion\": 1, \"mods\": [] }");
		return;
	}
	const required = ["name", "modId", "version", "purpose", "required", "scope", "channel", "licenseClass", "sizeMB", "candidate", "approvedBy", "approvedOn"];
	for (const m of reg.mods)
	{
		const id = m.modId || m.name || "?";
		for (const f of required)
			if (m[f] === undefined || m[f] === "")
				err("registry", `${id}: missing field "${f}"`);
		if (m.modId && !GUID_RE.test(m.modId))
			err("registry", `${id}: modId must be 16 uppercase hex chars`);
		if (m.version && !SEMVER_RE.test(m.version))
			err("registry", `${id}: version must be X.Y.Z`);
		if (m.scope && !["core", "content", "scenario", "server-only"].includes(m.scope))
			err("registry", `${id}: scope must be core|content|scenario|server-only`);
		if (m.channel && !["stable", "dev"].includes(m.channel))
			err("registry", `${id}: channel must be stable|dev`);
		if (m.licenseClass && !["GPL", "APL", "APL-ND", "custom"].includes(m.licenseClass))
			err("registry", `${id}: licenseClass must be GPL|APL|APL-ND|custom`);
		if (m.usageRestriction && !m.permissionRecord)
			err("registry", `${id}: usage-restricted mod needs a permissionRecord`);
		if (m.isCollection)
			err("registry", `${id}: collection items are not allowed; depend on individual modules`);
		if (m.candidate)
		{
			const cf = path.join(ROOT, "dependencies", m.candidate);
			const fm = fs.existsSync(cf) ? frontmatter(cf) : null;
			if (!fm)
				err("registry", `${id}: candidate file not found or has no frontmatter: dependencies/${m.candidate}`);
			else if (!["approved", "registered"].includes(fm.status) || !fm.approvedBy)
				err("registry", `${id}: candidate ${m.candidate} must have status approved/registered and approvedBy`);
		}
		if (m.modId)
			registry.set(m.modId, m);
	}
}

// ---------------------------------------------------------------- addons
const nmlGuids = new Map();
function checkAddons()
{
	const addonsDir = path.join(ROOT, "addons");
	if (!fs.existsSync(addonsDir))
		return;
	const dirs = fs.readdirSync(addonsDir, { withFileTypes: true }).filter((e) => e.isDirectory());
	for (const d of dirs)
	{
		const dir = path.join(addonsDir, d.name);
		if (!d.name.startsWith(TAG))
			err("addons", `addons/${d.name}: addon folders must start with ${TAG}`);
		if (fs.existsSync(path.join(dir, "Scripts", "WorkbenchGame", "EnfusionMCP")))
			err("addons", `addons/${d.name}/Scripts/WorkbenchGame/EnfusionMCP exists: EnfusionMCP handler scripts must never be committed or published (run wb_cleanup)`);
		const gproj = path.join(dir, `${d.name}.gproj`);
		if (!fs.existsSync(gproj))
		{
			err("gproj", `addons/${d.name}: missing ${d.name}.gproj`);
			continue;
		}
		const text = fs.readFileSync(gproj, "utf8");
		const id = /\bID\s+"([^"]+)"/.exec(text)?.[1];
		const guid = /\bGUID\s+"([^"]+)"/.exec(text)?.[1];
		if (id !== d.name)
			err("gproj", `${rel(gproj)}: ID "${id}" must equal the folder name "${d.name}"`);
		if (!guid || !GUID_RE.test(guid))
			err("gproj", `${rel(gproj)}: GUID missing or not 16 uppercase hex chars`);
		else if (nmlGuids.has(guid))
			err("gproj", `${rel(gproj)}: GUID ${guid} duplicates ${nmlGuids.get(guid).name}`);
		else
			nmlGuids.set(guid, { name: d.name, gproj, text });
	}
	for (const { name, gproj, text } of nmlGuids.values())
	{
		const deps = /Dependencies\s*\{([^}]*)\}/.exec(text)?.[1] || "";
		const guids = [...deps.matchAll(/"([0-9A-Fa-f]{16})"/g)].map((m) => m[1].toUpperCase());
		if (!guids.includes(BASE_GAME_GUID))
			err("gproj", `${rel(gproj)}: must depend on the base game (${BASE_GAME_GUID})`);
		for (const g of guids)
		{
			if (g === BASE_GAME_GUID || nmlGuids.has(g))
				continue;
			if (!registry.has(g))
				err("gproj", `${name}: dependency ${g} is not an approved entry in dependencies/mods.json`);
		}
	}
}

// ---------------------------------------------------------------- resources + meta
function checkResources()
{
	const files = walk(path.join(ROOT, "addons"));
	const guidSeen = new Map();
	const fileSet = new Set(files.map((f) => f.toLowerCase()));
	for (const f of files)
	{
		const r = rel(f);
		const ext = path.extname(f).toLowerCase();
		if (path.basename(f).startsWith("."))
			continue;
		if (ext === ".meta")
		{
			const target = f.slice(0, -5);
			if (!fileSet.has(target.toLowerCase()) && !fs.existsSync(target))
				err("meta", `orphan .meta (resource missing): ${r}`);
			const guid = /\{([0-9A-F]{16})\}/.exec(fs.readFileSync(f, "utf8"))?.[1];
			if (guid)
			{
				if (guidSeen.has(guid))
					err("meta", `duplicate resource GUID ${guid}: ${r} and ${guidSeen.get(guid)}`);
				else
					guidSeen.set(guid, r);
			}
			continue;
		}
		if (NO_META_EXT.includes(ext) || path.basename(f) === "resourceDatabase.rdb")
			continue;
		if (!fileSet.has(`${f}.meta`.toLowerCase()))
			err("meta", `resource has no .meta (register it in Workbench): ${r}`);
		if (CI && LFS_EXT.includes(ext))
		{
			const head = fs.readFileSync(f).subarray(0, 40).toString("utf8");
			if (!head.startsWith("version https://git-lfs"))
				err("lfs", `binary is not an LFS pointer (check .gitattributes / git lfs): ${r}`);
		}
	}
}

// ---------------------------------------------------------------- scripts
function checkScripts()
{
	const files = walk(path.join(ROOT, "addons")).filter((f) => f.toLowerCase().endsWith(".c"));
	for (const f of files)
	{
		const r = rel(f);
		const src = fs.readFileSync(f, "utf8")
			.replace(/\/\*[\s\S]*?\*\//g, "")
			.replace(/\/\/.*$/gm, "");
		const inModded = /\/Modded\//i.test(r);
		if (!path.basename(f).startsWith(TAG) && !inModded)
			err("script", `${r}: script file names must start with ${TAG}`);
		for (const m of src.matchAll(/^\s*(modded\s+)?(class|enum)\s+([A-Za-z_]\w*)/gm))
		{
			const [, modded, kind, name] = m;
			if (modded)
			{
				if (!inModded)
					err("script", `${r}: "modded class ${name}" must live under a Modded/ folder`);
				continue;
			}
			const want = kind === "enum" ? `${TAG}E` : TAG;
			if (!name.startsWith(want))
				err("script", `${r}: ${kind} ${name} must start with ${want}`);
		}
	}
}

// ---------------------------------------------------------------- server configs
const SECRET_KEY = /pass(word)?|token|secret|apikey/i;
function checkServerConfigs()
{
	const dir = path.join(ROOT, "server", "configs");
	if (!fs.existsSync(dir))
		return;
	for (const f of fs.readdirSync(dir).filter((n) => n.endsWith(".json")))
	{
		const file = path.join(dir, f);
		const cfg = readJson(file, "server");
		if (!cfg)
			continue;
		const pinned = /^(test|live)\b/i.test(f);
		const isLive = /^live\b/i.test(f);
		(function scan(o, p)
		{
			if (!o || typeof o !== "object")
				return;
			for (const [k, v] of Object.entries(o))
			{
				if (SECRET_KEY.test(k) && typeof v === "string" && v !== "" && !/^\$\{[A-Z0-9_]+\}$/.test(v))
					err("server", `${f}: ${p}${k} holds a literal secret; use a \${PLACEHOLDER} filled on the server`);
				if (k === "admins" && Array.isArray(v) && v.some((x) => !/^\$\{[A-Z0-9_]+\}$/.test(String(x))))
					err("server", `${f}: ${p}admins must only contain \${PLACEHOLDER} values`);
				scan(v, `${p}${k}.`);
			}
		})(cfg, "");
		let totalMB = 0;
		for (const mod of cfg.game?.mods || [])
		{
			const id = mod.modId;
			if (!id || !GUID_RE.test(id))
			{
				err("server", `${f}: mod entry has invalid modId: ${JSON.stringify(mod)}`);
				continue;
			}
			if (pinned && !mod.version)
				err("server", `${f}: mod ${id} (${mod.name}) must pin "version" (omitted = latest)`);
			const nml = nmlGuids.get(id);
			const reg = registry.get(id);
			if (!nml && !reg)
			{
				err("server", `${f}: mod ${id} (${mod.name}) is neither an NML addon nor in dependencies/mods.json`);
				continue;
			}
			if (reg)
			{
				totalMB += Number(reg.sizeMB) || 0;
				if (pinned && mod.version && mod.version !== reg.version)
					err("server", `${f}: mod ${id} pins ${mod.version} but the registry says ${reg.version}`);
				if (isLive && reg.channel === "dev" && !reg.liveDevChannelApproved)
					err("server", `${f}: mod ${id} is a Dev-channel build; LIVE needs liveDevChannelApproved in the registry`);
			}
		}
		info.push(`[size] ${f}: third-party download ${totalMB.toFixed(1)} MB (${(cfg.game?.mods || []).length} mods)`);
	}
}

// ---------------------------------------------------------------- candidates
function checkCandidates()
{
	const dir = path.join(ROOT, "dependencies", "candidates");
	if (!fs.existsSync(dir))
		return;
	for (const f of fs.readdirSync(dir).filter((n) => n.endsWith(".md") && !["README.md", "INDEX.md", "_TEMPLATE.md"].includes(n)))
	{
		const fm = frontmatter(path.join(dir, f));
		if (!fm)
		{
			err("candidate", `${f}: missing frontmatter`);
			continue;
		}
		for (const k of ["name", "status", "category", "proposedBy", "proposedOn"])
			if (!fm[k])
				err("candidate", `${f}: missing "${k}"`);
		if (fm.status && !CANDIDATE_STATUSES.includes(fm.status))
			err("candidate", `${f}: status "${fm.status}" must be one of ${CANDIDATE_STATUSES.join("|")}`);
		if (fm.category && !CATEGORIES.includes(fm.category))
			err("candidate", `${f}: category "${fm.category}" must be one of ${CATEGORIES.join("|")}`);
		if (["approved", "registered"].includes(fm.status) && !fm.approvedBy)
			err("candidate", `${f}: status ${fm.status} requires approvedBy`);
	}
}

checkRegistry();
checkAddons();
checkResources();
checkScripts();
checkServerConfigs();
checkCandidates();

for (const i of info)
	console.log(i);
for (const w of warnings)
	console.log(`WARN  ${w}`);
for (const e of errors)
	console.log(`ERROR ${e}`);
console.log(`\nNML validate: ${errors.length} error(s), ${warnings.length} warning(s) in ${ROOT}`);
process.exitCode = errors.length ? 1 : 0;
