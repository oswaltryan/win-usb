param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Files
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

. (Join-Path $PSScriptRoot "..\\lib\\tooling.ps1")

$clangFormat = Get-ClangFormatPath
if (-not $clangFormat) {
    throw "clang-format is required but not found in PATH or known install locations."
}

$targets = @()
if ($Files -and $Files.Count -gt 0) {
    $targets = $Files | Where-Object { $_ -match '\.(c|h)$' }
} else {
    $targets = Get-ChildItem `
        -Path (Join-Path $PSScriptRoot "..\\..\\src"), (Join-Path $PSScriptRoot "..\\..\\include") `
        -Recurse `
        -File `
        | Where-Object { $_.Extension -in ".c", ".h" } `
        | ForEach-Object { $_.FullName }
}

if ($targets.Count -eq 0) {
    exit 0
}

& $clangFormat --dry-run --Werror @targets
exit $LASTEXITCODE
