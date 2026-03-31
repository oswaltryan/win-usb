Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "..\\lib\\tooling.ps1")

$cppcheck = Get-CppcheckPath
if (-not $cppcheck) {
    throw "cppcheck is required but not found in PATH or known install locations."
}

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\\.."))
$includeDir = Join-Path $repoRoot "include"
$srcDir = Join-Path $repoRoot "src"
$testsDir = Join-Path $repoRoot "tests\\unit"

& $cppcheck `
    --enable=warning,performance,portability `
    --std=c11 `
    --error-exitcode=1 `
    --inline-suppr `
    --suppress=missingIncludeSystem `
    --suppress=unusedFunction `
    -I $includeDir `
    $srcDir `
    $testsDir

exit $LASTEXITCODE
