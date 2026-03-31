Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "..\\lib\\tooling.ps1")

$clangTidy = Get-ClangTidyPath
if (-not $clangTidy) {
    throw "clang-tidy is required but not found in PATH or known install locations."
}

$repoRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "..\\.."))
$includeDir = Join-Path $repoRoot "include"
$targets = Get-ChildItem `
    -Path (Join-Path $repoRoot "src") `
    -Recurse `
    -File `
    | Where-Object { $_.Extension -eq ".c" } `
    | ForEach-Object { $_.FullName }

if ($targets.Count -eq 0) {
    exit 0
}

$checks = "--checks=-*,clang-analyzer-*,performance-*,-clang-analyzer-security.insecureAPI.*"

$failed = $false
foreach ($file in $targets) {
    & $clangTidy `
        --quiet `
        $checks `
        --warnings-as-errors=* `
        $file `
        -- `
        -std=c11 `
        -I $includeDir `
        -D_CRT_SECURE_NO_WARNINGS `
        -D_WIN32_WINNT=0x0601 `
        -DWIN32 `
        -D_WINDOWS

    if ($LASTEXITCODE -ne 0) {
        $failed = $true
    }
}

if ($failed) {
    exit 1
}
exit 0
