# Start a local game client that joins the DEV server (tools/server-dev.ps1).
#
# Uses the diag exe (-client only works there), loads the NML addons from this
# checkout (-addonsDir) and runs windowed without stealing focus. Each client
# gets its own gitignored profile, so two clients can run side by side for the
# replication smoke test: -Instance 1 and -Instance 2.
#
# -IncludeVisuals also loads the optional client-only NML_Visuals addon (ADR 0007). -World <path> starts that
# world locally (single player, no server) instead of joining -Server.
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\client-dev.ps1 [-Instance <n>] [-Server <ip[:port]>] [-GamePath <dir>] [-IncludeTests] [-IncludeVisuals] [-World <path>] [-ExtraArgs <args>] [-ExtraAddons <guids>]

param(
	[string]$GamePath = $(if ($env:ENFUSION_GAME_PATH) { $env:ENFUSION_GAME_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger" }),
	[string]$Server = "127.0.0.1:2001",
	[int]$Instance = 1,
	# Also load NML_Tests (must match the server's -IncludeTests).
	[switch]$IncludeTests,
	# Also load NML_Visuals (client-only; servers never need it).
	[switch]$IncludeVisuals,
	# Start this world locally instead of joining -Server (plain path, e.g. Worlds/NML/Dev/NML_Dev_Everon.ent).
	[string]$World = "",
	# Extra engine startup parameters as one space-separated string, e.g. -ExtraArgs "-nmlVisualsCycle 4".
	# Split on spaces, so values must not contain spaces.
	[string]$ExtraArgs = "",
	# Comma-separated Workshop mod GUIDs already synced into .local\workshop (tools\workshop-sync.ps1) to load as well.
	[string]$ExtraAddons = ""
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
if ($IncludeVisuals) { $nmlAddons += ",81E04280BF506AF2" }
if ($ExtraAddons) { $nmlAddons = "$ExtraAddons,$nmlAddons"; $addons = "$addons," + (Join-Path $repo ".local\workshop\addons") }
if ($World) { $target = @("-world", "`"$World`""); $label = "world $World" } else { $target = @("-client", $Server); $label = $Server }
$argList = $target + @("-addonsDir", "`"$addons`"", "-addons", $nmlAddons, "-profile", "`"$profileDir`"",
	"-window", "-screenWidth", 1280, "-screenHeight", 720, "-noFocus", "-noSplash") + @($ExtraArgs -split " " | Where-Object { $_ })

Write-Host "Starting client $Instance -> $label`n  exe:     $exe`n  profile: $profileDir"
$proc = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $GamePath -PassThru
Write-Host "Client PID $($proc.Id). Logs: $profileDir\logs\<timestamp>\console.log"
