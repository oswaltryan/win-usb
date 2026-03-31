Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\\.."))
$buildScript = Join-Path $repoRoot "scripts\\build.ps1"

& $buildScript -Config Release
exit $LASTEXITCODE
