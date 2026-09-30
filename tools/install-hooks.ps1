# One-time setup per clone: enable Git LFS for this repository and install the
# NML pre-commit hook. Safe to re-run.
# Usage: powershell -ExecutionPolicy Bypass -File tools\install-hooks.ps1

$ErrorActionPreference = "Stop"
$repo = Split-Path -Parent $PSScriptRoot
Set-Location $repo

git lfs version | Out-Null
if ($LASTEXITCODE -ne 0) { throw "Git LFS is not installed. Install it from https://git-lfs.com and re-run." }

# --local keeps LFS configuration in this repository only (no global git changes).
git lfs install --local | Out-Null
Copy-Item -Force (Join-Path $repo "tools\githooks\pre-commit") (Join-Path $repo ".git\hooks\pre-commit")
Write-Host "Git LFS enabled for this repo and the NML pre-commit hook installed."
