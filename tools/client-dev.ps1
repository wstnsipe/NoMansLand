# Start a local game client that joins the DEV server (tools/server-dev.ps1).
#
# Uses the diag exe (-client only works there), loads the NML addons from this
# checkout (-addonsDir) and runs windowed without stealing focus. Each client
# gets its own gitignored profile, so two clients can run side by side for the
# replication smoke test: -Instance 1 and -Instance 2.
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\client-dev.ps1 [-Instance <n>] [-Server <ip[:port]>] [-GamePath <dir>] [-IncludeTests]

param(
	[string]$GamePath = $(if ($env:ENFUSION_GAME_PATH) { $env:ENFUSION_GAME_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger" }),
	[string]$Server = "127.0.0.1:2001",
	[int]$Instance = 1,
	# Also load NML_Tests (must match the server's -IncludeTests).
	[switch]$IncludeTests
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent $PSScriptRoot
$addons = Join-Path $repo "addons"
$profileDir = Join-Path $repo ".local\client$Instance\profile"
$exe = Join-Path $GamePath "ArmaReforgerSteamDiag.exe"

if (-not (Test-Path $exe)) { throw "ArmaReforgerSteamDiag.exe not found in $GamePath (set -GamePath or ENFUSION_GAME_PATH)." }
New-Item -ItemType Directory -Force -Path $profileDir | Out-Null

# NML runtime addons, in load order (same as tools/server-dev.ps1).
$nmlAddons = "C175C744D88BE5AE,8C44BDA9D3046928,2F33881926E82E22"
if ($IncludeTests) { $nmlAddons += ",99E85DF2DA22A8D2" }
$argList = @("-client", $Server, "-addonsDir", "`"$addons`"", "-addons", $nmlAddons, "-profile", "`"$profileDir`"",
	"-window", "-screenWidth", 1280, "-screenHeight", 720, "-noFocus", "-noSplash")

Write-Host "Starting client $Instance -> $Server`n  exe:     $exe`n  profile: $profileDir"
$proc = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $GamePath -PassThru
Write-Host "Client PID $($proc.Id). Logs: $profileDir\logs\<timestamp>\console.log"
