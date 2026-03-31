Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\\.."))
$testScript = Join-Path $repoRoot "scripts\\unit-test.ps1"

& $testScript -Config Release
exit $LASTEXITCODE
