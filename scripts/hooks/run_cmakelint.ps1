Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "..\\lib\\tooling.ps1")

$cmakelint = Get-CmakelintPath
if (-not $cmakelint) {
    throw "cmakelint is required but not found. Install with: python -m pip install --user cmakelint"
}

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\\.."))
$targets = @(
    (Join-Path $repoRoot "CMakeLists.txt")
)

& $cmakelint @targets
exit $LASTEXITCODE
