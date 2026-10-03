# Start a local Arma Reforger dedicated server (DEV only).
#
# World mode (use this until the NML addons are on the Workshop):
#   tools\server-dev.ps1 -World "Worlds/NML/Dev/NML_Dev_Everon.ent"
#   Runs ArmaReforgerServerDiag.exe -server <world> -addonsDir <repo>\addons -addons <NML GUIDs>.
#   The diag build is required: -client only works on the diag game exe, and the server rejects
#   diag clients unless it is diag too (RplConnection "isDevBinary" check). Pass the world as a
#   plain path; a {GUID} prefix is sent to clients verbatim and they cannot open it.
#
# Config mode (published builds only): tools\server-dev.ps1 [-Config <file>]
#   Runs ArmaReforgerServer.exe -config server\configs\dev.json. The engine refuses -config
#   together with -addons, and every config mod must be downloadable from the Workshop.
#
# The profile (logs, saves) goes to the gitignored <repo>\.local\server\profile.
# Ports: UDP 2001 (game), 17777 (A2S). The server listens on all interfaces.
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\server-dev.ps1 [-World <path>] [-Config <file>] [-ServerPath <dir>] [-ListScenarios] [-IncludeTests]

param(
	[string]$World = "",
	[string]$Config = "",
	[string]$ServerPath = $(if ($env:ARMA_SERVER_PATH) { $env:ARMA_SERVER_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger Server" }),
	[int]$MaxFps = 60,
	[switch]$ListScenarios,
	# Also load NML_Tests (never published). Only for test worlds such as Worlds/NML/Tests/NML_Test_Arsenal.ent.
	[switch]$IncludeTests
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent $PSScriptRoot
$addons = Join-Path $repo "addons"
$profileDir = Join-Path $repo ".local\server\profile"
# NML runtime addons, in load order: NML_Core, NML_Content, NML_Scenario_Dev (NML_Tests only with -IncludeTests, for test worlds).
$nmlAddons = "C175C744D88BE5AE,8C44BDA9D3046928,2F33881926E82E22"
if ($IncludeTests) { $nmlAddons += ",99E85DF2DA22A8D2" }

if ($World -and $Config) { throw "Use either -World (local addons) or -Config (published builds), not both." }

if ($World) {
	$exe = Join-Path $ServerPath "ArmaReforgerServerDiag.exe"
	$argList = @("-server", "`"$World`"", "-addonsDir", "`"$addons`"", "-addons", $nmlAddons,
		"-bindPort", 2001, "-a2sPort", 17777, "-profile", "`"$profileDir`"", "-maxFPS", $MaxFps)
	$source = "world $World"
} else {
	if (-not $Config) { $Config = Join-Path $repo "server\configs\dev.json" }
	if (-not (Test-Path $Config)) { throw "Server config not found: $Config" }
	$exe = Join-Path $ServerPath "ArmaReforgerServer.exe"
	$argList = @("-config", "`"$Config`"", "-profile", "`"$profileDir`"", "-addonsDir", "`"$addons`"", "-maxFPS", $MaxFps)
	$source = "config $Config"
}
if ($ListScenarios) { $argList += "-listScenarios" }

if (-not (Test-Path $exe)) { throw "$(Split-Path -Leaf $exe) not found in $ServerPath (Steam app 1874900; set -ServerPath or ARMA_SERVER_PATH)." }
New-Item -ItemType Directory -Force -Path $profileDir | Out-Null

Write-Host "Starting dedicated server:`n  exe:       $exe`n  source:    $source`n  addonsDir: $addons`n  profile:   $profileDir"
$proc = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $ServerPath -PassThru
Write-Host "Server PID $($proc.Id). Logs: $profileDir\logs\<timestamp>\console.log"
