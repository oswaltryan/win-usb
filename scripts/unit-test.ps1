param(
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$Config = "Release",
    [string]$BuildDir = "build",
    [string]$Generator = "",
    [ValidateSet("Win32", "x64", "ARM64")]
    [string]$Arch = "x64",
    [switch]$Clean,
    [Alias("h", "?")]
    [switch]$Help
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "lib\\common.ps1")

function Show-Usage {
    @"
Usage:
  .\scripts\unit-test.ps1 [options]

Options:
  -Help               Show this help text and exit.
  -Clean              Delete the build directory before configure.
  -Config <name>      Test configuration: Debug, Release, RelWithDebInfo, MinSizeRel.
  -BuildDir <path>    Build output directory (default: build).
  -Generator <name>   CMake generator (default: auto-detect Visual Studio).
  -Arch <arch>        Target architecture for Visual Studio generators: Win32, x64, ARM64.

Examples:
  .\scripts\unit-test.ps1
  .\scripts\unit-test.ps1 -Clean
  .\scripts\unit-test.ps1 -Config Debug
"@ | Write-Host
}

if ($Help) {
    Show-Usage
    return
}

$repoRoot = Split-Path -Parent $PSScriptRoot
$repoRoot = [System.IO.Path]::GetFullPath($repoRoot)
$buildScript = Join-Path $PSScriptRoot "build.ps1"
$buildDirPath = Resolve-BuildDirPath -RepoRoot $repoRoot -BuildDir $BuildDir

& $buildScript `
    -Config $Config `
    -BuildDir $BuildDir `
    -Generator $Generator `
    -Arch $Arch `
    -Clean:$Clean `
    -ConfigureOnly
if ($LASTEXITCODE -ne 0) {
    throw "Build configuration step failed."
}

& cmake --build $buildDirPath --config $Config --target winusb_unit_all
if ($LASTEXITCODE -ne 0) {
    throw "Unit-test build failed."
}

& ctest --test-dir $buildDirPath -C $Config --output-on-failure
if ($LASTEXITCODE -ne 0) {
    throw "Unit tests failed."
}
