// NML PreToolUse guard hook.
//
// Blocks writes outside the NML repository, writes to protected external paths
// (base game, Workbench profile incl. the EnfusionMCP support addon, npm cache,
// plugin cache, ~/.claude.json), and unsafe Enfusion MCP calls (missing
// projectPath/outputPath, wb_launch/wb_cleanup on non-NML addons).
//
// Contract (Claude Code hooks): JSON on stdin; exit 2 blocks the tool call and
// stderr is shown to Claude. Internal errors fail OPEN (exit 0 + warning) so a
// guard bug cannot brick sessions; permissions.deny in .claude/settings.json is
// the second, independent layer.
//
// Run the tests with: node tools/tests/guard.test.mjs

import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

const REPO = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..", "..");
const HOME = os.homedir();
const LOCALAPPDATA = process.env.LOCALAPPDATA || path.join(HOME, "AppData", "Local");
const DOCS = path.join(HOME, "Documents");

function norm(p, base = REPO)
{
	const abs = path.resolve(base, p);
	const s = abs.replace(/\\/g, "/").replace(/\/+$/, "");
	return process.platform === "win32" ? s.toLowerCase() : s;
}

function isUnder(p, root)
{
	return p === root || p.startsWith(root + "/");
}

const R = {
	repo: norm(REPO),
	addons: norm(path.join(REPO, "addons")),
	local: norm(path.join(REPO, ".local")),
	build: norm(path.join(REPO, "build")),
	server: norm(path.join(REPO, "server")),
	git: norm(path.join(REPO, ".git")),
	devGproj: norm(path.join(REPO, ".local", "NML_Dev", "NML_Dev.gproj")),
};

// Never writable, even if inside an allowed root.
const PROTECTED = [
	{ root: norm("C:/Program Files/Steam"), why: "Steam / base game / Arma Reforger Tools install" },
	{ root: norm("C:/Program Files (x86)/Steam"), why: "Steam install" },
	{ root: norm(path.join(DOCS, "My Games", "ArmaReforgerWorkbench")), why: "Workbench profile (EnfusionMCP support addon lives here)" },
	{ root: norm(path.join(DOCS, "My Games", "ArmaReforger")), why: "game profile / Workshop downloads / publish dir" },
	{ root: norm(path.join(LOCALAPPDATA, "npm-cache")), why: "npm cache (holds the patched enfusion-mcp package)" },
	{ root: norm(path.join(HOME, ".claude.json")), why: "Claude Code MCP/server config" },
	{ root: norm(path.join(HOME, ".claude", "plugins")), why: "installed Claude Code plugins (ECC, i-have-adhd)" },
	{ root: R.git, why: "git internals (use git commands, not file edits)" },
];

// Anything outside these is blocked for file-writing tools.
const ALLOWED_ROOTS = [
	R.repo,
	norm(path.join(LOCALAPPDATA, "Temp", "claude")),
	norm(path.join(HOME, ".claude")),
];

function protectedReason(p)
{
	if (/\/steamapps(\/|$)/.test(p))
		return "Steam library content (steamapps)";
	const hit = PROTECTED.find((x) => isUnder(p, x.root));
	return hit ? hit.why : null;
}

function checkWritePath(raw, cwd)
{
	if (!raw)
		return null;
	const p = norm(raw, cwd || REPO);
	const prot = protectedReason(p);
	if (prot)
		return `target is protected (${prot}): ${raw}`;
	if (!ALLOWED_ROOTS.some((r) => isUnder(p, r)))
		return `target is outside the NML repository and Claude scratch/config dirs: ${raw}`;
	return null;
}

function requireUnder(value, root, label)
{
	if (!value)
		return `${label} is required (defaults would write outside the NML repo)`;
	const p = norm(value);
	const prot = protectedReason(p);
	if (prot)
		return `${label} is protected (${prot}): ${value}`;
	if (!isUnder(p, root))
		return `${label} must be under ${root}: ${value}`;
	return null;
}

function requireAddonDir(value, label)
{
	const err = requireUnder(value, R.addons, label);
	if (err)
		return err;
	if (norm(value) === R.addons)
		return `${label} must be an addon folder (addons/NML_*), not the addons root: ${value}`;
	return null;
}

function checkEnfusion(tool, input)
{
	switch (tool)
	{
		case "project_write":
		{
			const base = input.projectPath;
			const err = requireUnder(base, R.repo, "projectPath");
			if (err)
				return err;
			if (!isUnder(norm(base), R.addons) && !isUnder(norm(base), R.local))
				return `projectPath must be under addons/ or .local/: ${base}`;
			if (input.path && !isUnder(norm(input.path, base), norm(base)))
				return `path escapes projectPath: ${input.path}`;
			return null;
		}
		case "mod_create":
		{
			const err = requireUnder(input.projectPath, R.addons, "projectPath");
			if (err)
				return err;
			if (norm(input.projectPath) !== R.addons)
				return `mod_create projectPath must be exactly ${R.addons}`;
			if (!/^NML_/.test(input.name || ""))
				return `addon name must start with the NML_ creator tag: ${input.name}`;
			return null;
		}
		case "script_create":
		{
			const err = requireAddonDir(input.projectPath, "projectPath");
			if (err)
				return err;
			if (input.scriptType !== "modded" && !/^NML_/.test(input.className || ""))
				return `className must start with the NML_ creator tag: ${input.className}`;
			return null;
		}
		case "prefab_create":
		case "config_create":
		case "layout_create":
			return requireAddonDir(input.projectPath, "projectPath");
		case "server_config":
			return requireUnder(input.projectPath, R.server, "projectPath");
		case "mod_build":
		{
			const err = requireUnder(input.outputPath, R.build, "outputPath");
			if (err)
				return err;
			if (input.gprojPath && !isUnder(norm(input.gprojPath), R.addons) && !isUnder(norm(input.gprojPath), R.local))
				return `gprojPath must be an NML addon or the local dev wrapper: ${input.gprojPath}`;
			return null;
		}
		case "wb_launch":
			if (!input.gprojPath || norm(input.gprojPath) !== R.devGproj)
				return `wb_launch must use the local dev wrapper (${R.devGproj}); prefer tools/wb-dev.ps1, which also passes -addonsDir`;
			return null;
		case "wb_cleanup":
		{
			const p = input.modDir ? norm(input.modDir) : "";
			if (!p || (!isUnder(p, R.addons) && !isUnder(p, R.local)) || p === R.addons)
				return `wb_cleanup modDir must be an NML addon or the local dev wrapper (never the EnfusionMCP support addon): ${input.modDir}`;
			return null;
		}
		default:
			return null;
	}
}

// Shell heuristic: block commands that mention a protected path AND contain a
// mutating verb. Reads (Get-Content, cat, ls, grep) of protected paths are allowed.
const MUTATING = /(\b(remove-item|rm|rmdir|rd|del|erase|set-content|add-content|out-file|clear-content|copy-item|move-item|new-item|rename-item|mv|cp|tee|robocopy|xcopy|mklink|icacls|attrib|takeown)\b|\bsed\s+-i|\bnpm\s+(install|i|uninstall|update|cache|ci)\b|>{1,2})/i;

function expandShellVars(cmd)
{
	return cmd
		.replace(/%userprofile%|\$env:userprofile|\$\{home\}|\$home\b/gi, HOME)
		.replace(/%localappdata%|\$env:localappdata/gi, LOCALAPPDATA)
		.replace(/(^|[\s"'=(])~(?=[\/\\])/g, (m, pre) => pre + HOME);
}

function checkShell(command)
{
	if (!command)
		return null;
	const cleaned = command.replace(/\d?>\s*(\$null|nul\b|\/dev\/null|&\d)/gi, "");
	let hay = expandShellVars(cleaned).replace(/\\/g, "/");
	hay = process.platform === "win32" ? hay.toLowerCase() : hay;
	if (!MUTATING.test(hay))
		return null;
	if (/\/steamapps\//.test(hay))
		return "shell command modifies Steam library content (steamapps)";
	for (const x of PROTECTED)
	{
		if (x.root === R.git)
			continue;
		if (hay.includes(x.root))
			return `shell command appears to modify a protected path (${x.why})`;
	}
	if (/\b(rm|remove-item|rmdir|rd|del)\b[^;|&\n]*(^|[\s"'\/])\.git([\/\s"']|$)/.test(hay))
		return "shell command appears to delete git internals (.git)";
	return null;
}

function decide(evt)
{
	const tool = evt.tool_name || "";
	const input = evt.tool_input || {};
	const FILE_TOOLS = { Edit: "file_path", Write: "file_path", MultiEdit: "file_path", NotebookEdit: "notebook_path" };
	if (FILE_TOOLS[tool])
		return checkWritePath(input[FILE_TOOLS[tool]], evt.cwd);
	if (tool === "Bash" || tool === "PowerShell")
		return checkShell(input.command);
	const m = /^mcp__enfusion[-_]mcp__(.+)$/.exec(tool);
	if (m)
		return checkEnfusion(m[1], input);
	return null;
}

try
{
	const raw = fs.readFileSync(0, "utf8");
	const evt = raw.trim() ? JSON.parse(raw) : {};
	const reason = decide(evt);
	if (reason)
	{
		process.stderr.write(
			`NML guard blocked ${evt.tool_name}: ${reason}\n` +
			"See CLAUDE.md > NML safeguards. If this change is really needed, ask the user to make it manually.\n",
		);
		process.exitCode = 2;
	}
}
catch (e)
{
	process.stderr.write(`NML guard warning (failing open): ${e && e.message ? e.message : e}\n`);
	process.exitCode = 0;
}
