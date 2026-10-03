# Shared by tools\server-dev.ps1 and tools\client-dev.ps1 (dot-sourced): resolve -Scenario <name>.
#
# Resolve-NmlScenario returns:
#   Guid       GUID of the scenario addon (loaded instead of NML_Scenario_Dev)
#   AddonsDir  value for -addonsDir: the repo addons folder, plus .local\workshop\addons (comma separated,
#              the form verified in Stage 6.3) when the scenario reaches any third-party Workshop dependency
# It throws, without starting anything, if the scenario addon does not exist or the Workshop folder is not synced yet.
function Resolve-NmlScenario {
	param([string]$Repo, [string]$Name)
	$addonsRoot = Join-Path $Repo "addons"
	$info = @{}
	foreach ($d in Get-ChildItem $addonsRoot -Directory) {
		$gproj = Join-Path $d.FullName "$($d.Name).gproj"
		if (-not (Test-Path $gproj)) { continue }
		$text = Get-Content $gproj -Raw
		$guid = [regex]::Match($text, 'GUID\s+"([0-9A-Fa-f]{16})"').Groups[1].Value.ToUpper()
		$deps = @()
		$block = [regex]::Match($text, 'Dependencies\s*\{([^}]*)\}')
		if ($block.Success) { $deps = @([regex]::Matches($block.Groups[1].Value, '"([0-9A-Fa-f]{16})"') | ForEach-Object { $_.Groups[1].Value.ToUpper() }) }
		$info[$d.Name] = [pscustomobject]@{ Guid = $guid; Deps = $deps }
	}
	$key = @($Name, "NML_Scenario_$Name") | Where-Object { $info.ContainsKey($_) } | Select-Object -First 1
	if (-not $key) { throw "Scenario addon '$Name' (or NML_Scenario_$Name) does not exist under addons\ (existing: $($info.Keys -join ', '))." }

	$byGuid = @{}
	foreach ($i in $info.Values) { $byGuid[$i.Guid] = $i }
	$baseGame = "58D0FB3206B6F859"
	$thirdParty = $false
	$seen = @{}
	$queue = New-Object System.Collections.Queue
	$queue.Enqueue($info[$key].Guid)
	while ($queue.Count -gt 0) {
		$g = $queue.Dequeue()
		if ($seen.ContainsKey($g)) { continue }
		$seen[$g] = $true
		foreach ($dep in $byGuid[$g].Deps) {
			if ($dep -eq $baseGame) { continue }
			if ($byGuid.ContainsKey($dep)) { $queue.Enqueue($dep) } else { $thirdParty = $true }
		}
	}

	$addonsDir = $addonsRoot
	if ($thirdParty) {
		$ws = Join-Path $Repo ".local\workshop\addons"
		if (-not (Test-Path $ws)) { throw "Scenario '$key' needs third-party Workshop addons but $ws does not exist. Run: powershell -File tools\workshop-sync.ps1 -Scenario $Name" }
		$addonsDir = "$addonsRoot,$ws"
	}
	return [pscustomobject]@{ Guid = $info[$key].Guid; AddonsDir = $addonsDir }
}
