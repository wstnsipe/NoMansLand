// Tests for .claude/hooks/nml-guard.mjs.
// Runs the guard exactly the way Claude Code does (the node -e bootstrap from
// .claude/settings.json) and checks exit codes: 2 = blocked, 0 = allowed.
// Usage: node tools/tests/guard.test.mjs

import { spawnSync } from "node:child_process";
import fs from "node:fs";
import os from "node:os";
import path from "node:path";
import { fileURLToPath } from "node:url";

const REPO = path.resolve(path.dirname(fileURLToPath(import.meta.url)), "..", "..");
const HOME = os.homedir();
const DOCS = path.join(HOME, "Documents");
const LOCALAPPDATA = process.env.LOCALAPPDATA || path.join(HOME, "AppData", "Local");

const settings = JSON.parse(fs.readFileSync(path.join(REPO, ".claude", "settings.json"), "utf8"));
const guardHook = settings.hooks.PreToolUse
	.flatMap((h) => h.hooks)
	.find((h) => h.command.includes("nml-guard.mjs"));
if (!guardHook)
	throw new Error("nml-guard hook not registered in .claude/settings.json");

function run(evt)
{
	// The settings command is a shell string: node -e "<js>". Extract the JS so the
	// test does not depend on the shell's quoting rules.
	const js = /node -e "(.*)"$/s.exec(guardHook.command)[1].replace(/\\"/g, "\"");
	const r = spawnSync(process.execPath, ["-e", js], {
		input: JSON.stringify(evt),
		env: { ...process.env, CLAUDE_PROJECT_DIR: REPO },
		encoding: "utf8",
	});
	return { code: r.status, err: r.stderr };
}

const J = (...p) => path.join(...p);
const WB = J(DOCS, "My Games", "ArmaReforgerWorkbench", "addons");
const cases = [
	// File tools
	["Write inside repo", { tool_name: "Write", tool_input: { file_path: J(REPO, "docs", "x.md") } }, 0],
	["Edit repo CLAUDE.md", { tool_name: "Edit", tool_input: { file_path: J(REPO, "CLAUDE.md") } }, 0],
	["Write relative path (cwd=repo)", { tool_name: "Write", tool_input: { file_path: "docs/y.md" }, cwd: REPO }, 0],
	["Write scratchpad", { tool_name: "Write", tool_input: { file_path: J(LOCALAPPDATA, "Temp", "claude", "s", "a.txt") } }, 0],
	["Write plan file", { tool_name: "Write", tool_input: { file_path: J(HOME, ".claude", "plans", "p.md") } }, 0],
	["Write Program Files Steam", { tool_name: "Write", tool_input: { file_path: "C:/Program Files/Steam/steamapps/common/Arma Reforger/x.c" } }, 2],
	["Edit EnfusionMCP support addon", { tool_name: "Edit", tool_input: { file_path: J(WB, "EnfusionMCP", "EnfusionMCP.gproj") } }, 2],
	["Write other steam library", { tool_name: "Write", tool_input: { file_path: "D:/SteamLibrary/steamapps/workshop/x" } }, 2],
	["Edit npm cache", { tool_name: "Edit", tool_input: { file_path: J(LOCALAPPDATA, "npm-cache", "_npx", "a", "vfs.js") } }, 2],
	["Edit ~/.claude.json", { tool_name: "Edit", tool_input: { file_path: J(HOME, ".claude.json") } }, 2],
	["Edit plugin cache", { tool_name: "Edit", tool_input: { file_path: J(HOME, ".claude", "plugins", "cache", "ecc", "x.json") } }, 2],
	["Write .git internals", { tool_name: "Write", tool_input: { file_path: J(REPO, ".git", "config") } }, 2],
	["Write unrelated dir", { tool_name: "Write", tool_input: { file_path: "C:/Users/Public/x.txt" } }, 2],
	["Write via .. escape", { tool_name: "Write", tool_input: { file_path: J(REPO, "..", "Other", "x.txt") } }, 2],
	["NotebookEdit outside", { tool_name: "NotebookEdit", tool_input: { notebook_path: "C:/Temp/n.ipynb" } }, 2],
	// Enfusion MCP
	["project_write no projectPath", { tool_name: "mcp__enfusion-mcp__project_write", tool_input: { path: "a.c", content: "" } }, 2],
	["project_write NML addon", { tool_name: "mcp__enfusion-mcp__project_write", tool_input: { projectPath: J(REPO, "addons", "NML_Core"), path: "Scripts/Game/NML/a.c", content: "" } }, 0],
	["project_write escape", { tool_name: "mcp__enfusion-mcp__project_write", tool_input: { projectPath: J(REPO, "addons", "NML_Core"), path: "../../../x.c", content: "" } }, 2],
	["project_write EnfusionMCP", { tool_name: "mcp__enfusion-mcp__project_write", tool_input: { projectPath: J(WB, "EnfusionMCP"), path: "x.c", content: "" } }, 2],
	["mod_create default path", { tool_name: "mcp__enfusion-mcp__mod_create", tool_input: { name: "NML_Core", description: "d" } }, 2],
	["mod_create NML ok", { tool_name: "mcp__enfusion-mcp__mod_create", tool_input: { name: "NML_Core", description: "d", projectPath: J(REPO, "addons") } }, 0],
	["mod_create untagged name", { tool_name: "mcp__enfusion-mcp__mod_create", tool_input: { name: "Core", description: "d", projectPath: J(REPO, "addons") } }, 2],
	["script_create untagged class", { tool_name: "mcp__enfusion-mcp__script_create", tool_input: { className: "LootComponent", scriptType: "component", projectPath: J(REPO, "addons", "NML_Core") } }, 2],
	["script_create tagged class", { tool_name: "mcp__enfusion-mcp__script_create", tool_input: { className: "NML_LootComponent", scriptType: "component", projectPath: J(REPO, "addons", "NML_Core") } }, 0],
	["script_create addons root", { tool_name: "mcp__enfusion-mcp__script_create", tool_input: { className: "NML_X", scriptType: "basic", projectPath: J(REPO, "addons") } }, 2],
	["prefab_create no projectPath", { tool_name: "mcp__enfusion-mcp__prefab_create", tool_input: { name: "X", prefabType: "generic" } }, 2],
	["mod_build no outputPath", { tool_name: "mcp__enfusion-mcp__mod_build", tool_input: { addonName: "NML_Core" } }, 2],
	["mod_build output in build/", { tool_name: "mcp__enfusion-mcp__mod_build", tool_input: { addonName: "NML_Core", outputPath: J(REPO, "build", "NML_Core") } }, 0],
	["mod_build output in Program Files", { tool_name: "mcp__enfusion-mcp__mod_build", tool_input: { addonName: "NML_Core", outputPath: "C:/Program Files/Steam/steamapps/common/Arma Reforger Tools/addons/x" } }, 2],
	["server_config in server/", { tool_name: "mcp__enfusion-mcp__server_config", tool_input: { name: "t", projectPath: J(REPO, "server") } }, 0],
	["server_config default", { tool_name: "mcp__enfusion-mcp__server_config", tool_input: { name: "t" } }, 2],
	["wb_launch no gproj", { tool_name: "mcp__enfusion-mcp__wb_launch", tool_input: {} }, 2],
	["wb_launch EnfusionMCP", { tool_name: "mcp__enfusion-mcp__wb_launch", tool_input: { gprojPath: J(WB, "EnfusionMCP", "EnfusionMCP.gproj") } }, 2],
	["wb_launch dev wrapper", { tool_name: "mcp__enfusion-mcp__wb_launch", tool_input: { gprojPath: J(REPO, ".local", "NML_Dev", "NML_Dev.gproj") } }, 0],
	["wb_cleanup EnfusionMCP", { tool_name: "mcp__enfusion-mcp__wb_cleanup", tool_input: { modDir: J(WB, "EnfusionMCP") } }, 2],
	["wb_cleanup NML addon", { tool_name: "mcp__enfusion-mcp__wb_cleanup", tool_input: { modDir: J(REPO, "addons", "NML_Core") } }, 0],
	["read-only MCP tool", { tool_name: "mcp__enfusion-mcp__api_search", tool_input: { query: "x" } }, 0],
	// Shell
	["shell read of protected path", { tool_name: "PowerShell", tool_input: { command: `Get-Content "${J(WB, "EnfusionMCP", "EnfusionMCP.gproj")}"` } }, 0],
	["shell delete EnfusionMCP", { tool_name: "PowerShell", tool_input: { command: `Remove-Item -Recurse "${J(WB, "EnfusionMCP")}"` } }, 2],
	["shell delete via $env var", { tool_name: "PowerShell", tool_input: { command: "Remove-Item \"$env:USERPROFILE\\Documents\\My Games\\ArmaReforgerWorkbench\\addons\\EnfusionMCP\" -Recurse" } }, 2],
	["bash rm via ~", { tool_name: "Bash", tool_input: { command: "rm -rf ~/AppData/Local/npm-cache/_npx" } }, 2],
	["shell redirect into steamapps", { tool_name: "Bash", tool_input: { command: "echo x > '/c/Program Files/Steam/steamapps/common/x.txt'" } }, 2],
	["shell read with 2>$null", { tool_name: "PowerShell", tool_input: { command: `Get-ChildItem "${J(WB)}" 2>$null` } }, 0],
	["shell rm .git", { tool_name: "Bash", tool_input: { command: "rm -rf .git" } }, 2],
	["shell git commit", { tool_name: "Bash", tool_input: { command: "git commit -m \"x\"" } }, 0],
	["shell repo write", { tool_name: "PowerShell", tool_input: { command: `Set-Content "${J(REPO, "build", "x.txt")}" "x"` } }, 0],
	// Robustness
	["empty stdin", {}, 0],
];

let fail = 0;
for (const [name, evt, expected] of cases)
{
	const r = run(evt);
	const ok = r.code === expected;
	if (!ok)
		fail++;
	console.log(`${ok ? "PASS" : "FAIL"}  ${name}  (exit ${r.code}, expected ${expected})${ok ? "" : "\n      " + r.err.trim()}`);
}
console.log(`\n${cases.length - fail}/${cases.length} passed`);
process.exitCode = fail ? 1 : 0;
