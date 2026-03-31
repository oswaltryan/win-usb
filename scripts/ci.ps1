param(
    [switch]$Quick,
    [Alias("h", "?")]
    [switch]$Help
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($Help) {
    @"
Usage:
  .\scripts\ci.ps1 [options]

Options:
  -Quick     Run pre-commit stage only (format/lint checks).
  -Help      Show this help text and exit.

Default behavior:
  1) pre-commit stage checks
  2) pre-push stage checks (build, tests, static analysis)
"@ | Write-Host
    return
}

& python -m pre_commit run --all-files --hook-stage pre-commit
if ($LASTEXITCODE -ne 0) {
    throw "pre-commit stage checks failed."
}

if (-not $Quick) {
    & python -m pre_commit run --all-files --hook-stage pre-push
    if ($LASTEXITCODE -ne 0) {
        throw "pre-push stage checks failed."
    }
}
