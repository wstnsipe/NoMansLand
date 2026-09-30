# Launch Arma Reforger Workbench on the local NML dev wrapper.
#
# Why this exists (Phase 0 finding): the Enfusion MCP's wb_launch cannot pass
# -addonsDir, so Workbench would not find the NML addons in this repository.
# This script starts Workbench with:
#   -gproj    <repo>\.local\NML_Dev\NML_Dev.gproj   (local-only wrapper, gitignored)
#   -addonsDir <repo>\addons                         (resolves NML_Core, NML_Content, ...)
# The MCP then attaches automatically: wb_launch returns early when the Net API
# (port 5775) is already answering. EnfusionMCP handler scripts belong in the
# wrapper (.local/), never in addons/.
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\wb-dev.ps1 [-ToolsPath <dir>] [-GamePath <dir>] [-McpHandlers <dir>] [-NoWait]

param(
	[string]$ToolsPath = $(if ($env:ENFUSION_WORKBENCH_PATH) { $env:ENFUSION_WORKBENCH_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger Tools" }),
	[string]$GamePath = $(if ($env:ENFUSION_GAME_PATH) { $env:ENFUSION_GAME_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger" }),
	[string]$McpHandlers = $env:ENFUSION_MCP_HANDLERS,
	[int]$Port = 5775,
	[switch]$NoWait
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent $PSScriptRoot
$wrapper = Join-Path $repo ".local\NML_Dev\NML_Dev.gproj"
$addons = Join-Path $repo "addons"

$exe = @(
	(Join-Path $ToolsPath "Workbench\ArmaReforgerWorkbenchSteamDiag.exe"),
	(Join-Path $ToolsPath "ArmaReforgerWorkbenchSteamDiag.exe")
) | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not $exe) { throw "ArmaReforgerWorkbenchSteamDiag.exe not found under $ToolsPath (set -ToolsPath or ENFUSION_WORKBENCH_PATH)." }
if (-not (Test-Path $wrapper)) { throw "Dev wrapper not found: $wrapper. It is created in Phase 2 (see docs/workflows.md)." }
if (-not (Test-Path $addons)) { throw "Addons folder not found: $addons" }
if (-not (Test-Path (Join-Path $GamePath "addons"))) { throw "Game install not found at $GamePath (set -GamePath or ENFUSION_GAME_PATH)." }

if (-not $McpHandlers) {
	# Newest npx copy of the enfusion-mcp package (read-only source; we only copy from it).
	$McpHandlers = Get-ChildItem (Join-Path $env:LOCALAPPDATA "npm-cache\_npx\*\node_modules\enfusion-mcp\mod\Scripts\WorkbenchGame\EnfusionMCP") -Directory -ErrorAction SilentlyContinue |
		Sort-Object LastWriteTime -Descending | Select-Object -First 1 -ExpandProperty FullName
}

if (Get-Process -ErrorAction SilentlyContinue | Where-Object { $_.Name -like "ArmaReforgerWorkbench*" }) {
	Write-Warning "Workbench is already running. Close it first so it restarts with -addonsDir."
	exit 1
}

# Install the MCP handler scripts into the wrapper (what wb_launch would do), so the
# wb_* tools work and the MCP never falls back to its kill-and-relaunch recovery,
# which would restart Workbench without -addonsDir.
if ($McpHandlers -and (Test-Path $McpHandlers)) {
	$handlerTarget = Join-Path (Split-Path -Parent $wrapper) "Scripts\WorkbenchGame\EnfusionMCP"
	New-Item -ItemType Directory -Force -Path $handlerTarget | Out-Null
	Copy-Item -Path (Join-Path $McpHandlers "*.c") -Destination $handlerTarget -Force
	Write-Host "MCP handlers copied into the wrapper: $handlerTarget"
} else {
	Write-Warning "enfusion-mcp handler scripts not found (pass -McpHandlers <dir>); wb_* tools will not work."
}

Write-Host "Starting Workbench:`n  exe:       $exe`n  gproj:     $wrapper`n  addonsDir: $addons"
# The game folder is the working directory so Workbench resolves the base game (ArmaReforger.gproj).
$proc = Start-Process -FilePath $exe -ArgumentList "-gproj `"$wrapper`" -addonsDir `"$addons`"" -WorkingDirectory $GamePath -PassThru
Write-Host "Workbench PID $($proc.Id)"

if ($NoWait) { exit 0 }
for ($i = 0; $i -lt 60; $i++) {
	if (Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue) {
		Write-Host "Net API is listening on port $Port - the Enfusion MCP can connect."
		exit 0
	}
	Start-Sleep -Seconds 3
}
Write-Warning "Net API did not open on port $Port within 180s. Check File > Options > General > Net API in Workbench."
exit 1
