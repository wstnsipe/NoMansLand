# Download the pinned third-party Workshop closure for a scenario into the gitignored .local\workshop.
#
# Official mechanism only: the dedicated server (ArmaReforgerServer.exe, config mode) downloads the
# mods listed in game.mods at their exact `version` into -addonDownloadDir; this script starts it with a
# throwaway config, waits for "Required addons are ready to use", stops it, then verifies the result.
# Nothing is copied into addons\ and nothing is read from the game's own Documents\...\addons folder.
#
# Verified behaviour (Stage 6.3 probe, Warfare-Colormod 65A2EA40DC9E632A, game 1.8.0.13):
#   - game.mods version "1.0.12" while 1.0.13 was current: the server downloaded exactly 1.0.12
#     (log "Downloading <id> version 1.0.12"; files carry *_1.0.12_manifest.json).
#   - a version that does not exist ("9.9.9"): "Attempt to download an empty package", "Failed to fetch
#     addon details from workshop API", "Unable to initialize the game"; nothing is downloaded.
#   - Workshop keeps only the last 50 versions of a mod (BI wiki), and removed versions are deleted, so
#     an exact historical version is NOT guaranteed to stay available. A pin that disappears fails closed.
#   - Not verified: whether the server resolves a mod's own dependencies at their latest version when
#     they are not listed. TEST/LIVE lists therefore carry the full closure (tools/validate.mjs checks it).
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\workshop-sync.ps1 (-Scenario <name> | -Root <GUID[,GUID]>) [-Verify] [-DryRun]
#   -Scenario  addon name under addons\ (Myrove or NML_Scenario_Myrove); its registry closure is synced
#   -Root      registry mod GUID(s) to sync with their closure (usable before the scenario addon exists)
#   -Verify    only check .local\workshop against the registry pins; no download, exit 1 on any mismatch
#   -DryRun    print the plan and the config, start nothing
#   -ModListJson  (probe/test only) use this JSON array of {modId,name,version} instead of the registry
#   -WorkshopDir  download folder, must be under <repo>\.local (default .local\workshop)

param(
	[string]$Scenario = "",
	[string]$Root = "",
	[switch]$Verify,
	[switch]$DryRun,
	[string]$ModListJson = "",
	[string]$WorkshopDir = "",
	[string]$ServerPath = $(if ($env:ARMA_SERVER_PATH) { $env:ARMA_SERVER_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger Server" }),
	[int]$TimeoutSec = 3600
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent $PSScriptRoot
$local = Join-Path $repo ".local"
if (-not $WorkshopDir) { $WorkshopDir = Join-Path $local "workshop" }
$WorkshopDir = [System.IO.Path]::GetFullPath($WorkshopDir)
if (-not $WorkshopDir.StartsWith([System.IO.Path]::GetFullPath($local) + "\", [System.StringComparison]::OrdinalIgnoreCase)) {
	throw "WorkshopDir must be under $local (gitignored); got $WorkshopDir"
}

# ---- Which mods, at which versions
if ($ModListJson) {
	Write-Warning "Using $ModListJson instead of the registry (probe/test only)."
	$mods = @(Get-Content $ModListJson -Raw | ConvertFrom-Json)
} else {
	if ($Scenario -and $Root) { throw "Use either -Scenario or -Root, not both." }
	if (-not $Scenario -and -not $Root) { throw "Give -Scenario <name> or -Root <GUID[,GUID]>." }
	$sel = if ($Scenario) { @("--scenario", $Scenario) } else { @("--root", $Root) }
	$json = & node (Join-Path $PSScriptRoot "modlist.mjs") @sel "--format" "json"
	if ($LASTEXITCODE -ne 0) { throw "tools\modlist.mjs failed (see message above); nothing was downloaded." }
	$mods = @($json -join "`n" | ConvertFrom-Json)
}
if ($mods.Count -eq 0) { throw "The mod list is empty." }
Write-Host "Workshop sync: $($mods.Count) mod(s) -> $WorkshopDir"
foreach ($m in $mods) { Write-Host ("  {0}  {1}  {2}" -f $m.modId, $m.version, $m.name) }

# ---- Verify what is on disk against the pins
function Test-WorkshopDir {
	$bad = 0
	foreach ($m in $mods) {
		$dir = Get-ChildItem (Join-Path $WorkshopDir "addons") -Directory -Filter "*_$($m.modId)" -ErrorAction SilentlyContinue | Select-Object -First 1
		if (-not $dir) { Write-Host "MISSING   $($m.modId) $($m.name)"; $bad++; continue }
		$versions = @(Get-ChildItem $dir.FullName -File -Filter "*_manifest.json" | ForEach-Object { if ($_.Name -match '_(\d+\.\d+\.\d+)_manifest\.json$') { $Matches[1] } } | Sort-Object -Unique)
		if ($versions -contains $m.version) {
			Write-Host "OK        $($m.modId) $($m.version) $($m.name)"
			if ($versions.Count -gt 1) { Write-Warning "$($m.name): other version manifests are present ($($versions -join ', ')); delete $($dir.FullName) and re-sync for a clean copy." }
		}
		else { Write-Host "MISMATCH  $($m.modId) $($m.name): pinned $($m.version), on disk: $($versions -join ', ')"; $bad++ }
	}
	return $bad
}

if ($Verify) {
	$bad = Test-WorkshopDir
	if ($bad -gt 0) { Write-Host "`n$bad problem(s). Run without -Verify to download."; exit 1 }
	Write-Host "`nAll $($mods.Count) mod(s) match their pins."
	exit 0
}

# ---- Throwaway server config: localhost only, not listed, no passwords
$syncDir = Join-Path $WorkshopDir "sync"
$cfg = [ordered]@{
	bindAddress = "127.0.0.1"; bindPort = 2011; publicPort = 2011
	a2s = [ordered]@{ address = "127.0.0.1"; port = 17787 }
	game = [ordered]@{
		name = "NML workshop sync"; password = ""; scenarioId = "{ECC61978EDCC2B5A}Missions/23_Campaign.conf"
		maxPlayers = 2; visible = $false; supportedPlatforms = @("PLATFORM_PC")
		gameProperties = [ordered]@{ serverMaxViewDistance = 1600; battlEye = $false; fastValidation = $true }
		mods = @($mods | ForEach-Object { [ordered]@{ modId = $_.modId; name = $_.name; version = $_.version } })
	}
}
$cfgPath = Join-Path $syncDir "sync-config.json"
$exe = Join-Path $ServerPath "ArmaReforgerServer.exe"
$argList = @("-config", "`"$cfgPath`"", "-profile", "`"$syncDir\profile`"", "-addonDownloadDir", "`"$WorkshopDir`"", "-maxFPS", 30)

if ($DryRun) {
	Write-Host "`nDRY RUN: would start`n  $exe $($argList -join ' ')`nwith config:"
	$cfg | ConvertTo-Json -Depth 8
	exit 0
}
if (-not (Test-Path $exe)) { throw "ArmaReforgerServer.exe not found in $ServerPath (Steam app 1874900; set -ServerPath or ARMA_SERVER_PATH)." }
if (Get-Process -Name ArmaReforgerServer, ArmaReforgerServerDiag -ErrorAction SilentlyContinue) { throw "A dedicated server is already running; stop it first (ports 2011/17787 and the download folder must be free)." }

New-Item -ItemType Directory -Force -Path $syncDir | Out-Null
$cfg | ConvertTo-Json -Depth 8 | Set-Content $cfgPath -Encoding utf8
Write-Host "`nStarting the official download (large closures take a while)..."
$proc = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $ServerPath -PassThru
$started = Get-Date
$ready = $false
while (((Get-Date) - $started).TotalSeconds -lt $TimeoutSec) {
	Start-Sleep 3
	$log = Get-ChildItem "$syncDir\profile\logs" -Recurse -Filter console.log -ErrorAction SilentlyContinue | Where-Object { $_.LastWriteTime -ge $started.AddSeconds(-5) } | Sort-Object LastWriteTime -Descending | Select-Object -First 1
	if ($log -and (Select-String -Path $log.FullName -Pattern "Required addons are ready to use" -Quiet)) { $ready = $true; break }
	if ($proc.HasExited) { break }
}
if (-not $proc.HasExited) { Stop-Process -Id $proc.Id -Force }
if (-not $ready) {
	Write-Host "Download did not finish: server exited or timed out. Log: $($log.FullName)"
	if ($log) { node (Join-Path $PSScriptRoot "log-scan.mjs") $log.FullName }
	exit 1
}
$bad = Test-WorkshopDir
if ($bad -gt 0) { Write-Host "`n$bad problem(s) after download."; exit 1 }
Write-Host "`nDone. Use tools\server-dev.ps1 -Scenario <name> -World <world> to load from $WorkshopDir\addons."
