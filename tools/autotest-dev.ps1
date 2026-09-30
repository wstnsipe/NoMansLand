# Run NML_Tests Autotest suites on the diag game exe and print the JUnit result.
#
# The Autotest runner (base game SCR_TestRunner) reads -autotest <suite|case|{group conf GUID}>,
# opens the suite's world, runs the tests, writes $logs:/junit.xml and exits.
# Loads all four NML addons from this checkout; the profile goes to the gitignored .local\autotest.
#
# Usage: powershell -ExecutionPolicy Bypass -File tools\autotest-dev.ps1 [-Test <SuiteOrCaseClass>] [-GamePath <dir>] [-TimeoutSec <n>]
# Exit code: 0 = all tests passed, 1 = failures/errors or no report.

param(
	[string]$Test = "NML_TEST_DevScenarioSuite",
	[string]$GamePath = $(if ($env:ENFUSION_GAME_PATH) { $env:ENFUSION_GAME_PATH } else { "C:\Program Files\Steam\steamapps\common\Arma Reforger" }),
	[int]$TimeoutSec = 600
)

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent $PSScriptRoot
$addons = Join-Path $repo "addons"
$profileDir = Join-Path $repo ".local\autotest\profile"
$exe = Join-Path $GamePath "ArmaReforgerSteamDiag.exe"

if (-not (Test-Path $exe)) { throw "ArmaReforgerSteamDiag.exe not found in $GamePath (set -GamePath or ENFUSION_GAME_PATH)." }
New-Item -ItemType Directory -Force -Path $profileDir | Out-Null

# NML_Core, NML_Content, NML_Scenario_Dev, NML_Tests
$nmlAddons = "C175C744D88BE5AE,8C44BDA9D3046928,2F33881926E82E22,99E85DF2DA22A8D2"
$argList = @("-autotest", $Test, "-addonsDir", "`"$addons`"", "-addons", $nmlAddons, "-profile", "`"$profileDir`"",
	"-window", "-screenWidth", 1280, "-screenHeight", 720, "-noFocus", "-noSplash")

Write-Host "Running Autotest '$Test'`n  exe:     $exe`n  profile: $profileDir"
$started = Get-Date
$proc = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $GamePath -PassThru
if (-not $proc.WaitForExit($TimeoutSec * 1000)) {
	Stop-Process -Id $proc.Id -Force -ErrorAction SilentlyContinue
	Write-Warning "Autotest did not finish within $TimeoutSec s; process stopped."
}

$logDir = Get-ChildItem (Join-Path $profileDir "logs") -Directory -ErrorAction SilentlyContinue |
	Where-Object { $_.LastWriteTime -ge $started } | Sort-Object LastWriteTime -Descending | Select-Object -First 1
$junit = if ($logDir) { Join-Path $logDir.FullName "junit.xml" } else { $null }
if (-not $junit -or -not (Test-Path $junit) -or (Get-Item $junit).LastWriteTime -lt $started) {
	Write-Warning "No junit.xml produced by this run. Check $profileDir\logs\<timestamp>\console.log"
	exit 1
}

$xml = New-Object System.Xml.XmlDocument
try { $xml.Load($junit) } catch {
	Write-Warning "Unreadable junit.xml ($($_.Exception.Message)): $junit"
	exit 1
}
$suites = @($xml.SelectNodes("//testsuite"))
$cases = @($xml.SelectNodes("//testcase"))
# Node test, not property truthiness: an empty <failure/> must still count as a failure.
$isFailed = { param($case) $null -ne $case.SelectSingleNode("failure|error") }
$failed = @($cases | Where-Object { & $isFailed $_ })
foreach ($c in $cases) {
	$state = if (& $isFailed $c) { "FAIL" } else { "PASS" }
	Write-Host ("{0}  {1}" -f $state, $c.name)
}
Write-Host ("{0} suite(s), {1} case(s), {2} failed. Report: {3}" -f $suites.Count, $cases.Count, $failed.Count, $junit)
if ($cases.Count -eq 0 -or $failed.Count -gt 0) { exit 1 }
exit 0
