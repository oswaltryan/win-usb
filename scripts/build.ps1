param(
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$Config = "Release",
    [string]$BuildDir = "build",
    [string]$Generator = "",
    [ValidateSet("Win32", "x64", "ARM64")]
    [string]$Arch = "x64",
    [switch]$Clean,
    [switch]$ConfigureOnly,
    [Alias("h", "?")]
    [switch]$Help
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
. (Join-Path $PSScriptRoot "lib\\common.ps1")

function Show-Usage {
    @"
Usage:
  .\scripts\build.ps1 [options]

Options:
  -Help               Show this help text and exit.
  -Clean              Delete the build directory before configure.
  -ConfigureOnly      Run CMake configure only (skip compile).
  -Config <name>      Build configuration: Debug, Release, RelWithDebInfo, MinSizeRel.
  -BuildDir <path>    Build output directory (default: build).
  -Generator <name>   CMake generator (default: auto-detect Visual Studio).
  -Arch <arch>        Target architecture for Visual Studio generators: Win32, x64, ARM64.

Examples:
  .\scripts\build.ps1
  .\scripts\build.ps1 -Clean
  .\scripts\build.ps1 -Config Debug
  .\scripts\build.ps1 -Generator "Visual Studio 18 2026" -Arch x64
"@ | Write-Host
}

if ($Help) {
    Show-Usage
    return
}

$repoRoot = Split-Path -Parent $PSScriptRoot
$repoRoot = [System.IO.Path]::GetFullPath($repoRoot)
$buildDirPath = Resolve-BuildDirPath -RepoRoot $repoRoot -BuildDir $BuildDir

if ($Clean -and (Test-Path $buildDirPath)) {
    Remove-Item -LiteralPath $buildDirPath -Recurse -Force
}

if (-not $Generator -and $env:CMAKE_GENERATOR) {
    $Generator = $env:CMAKE_GENERATOR
}
if (-not $Generator) {
    $Generator = Get-DefaultCMakeGenerator
}

$configureArgs = @("-S", $repoRoot, "-B", $buildDirPath)
if ($Generator) {
    $configureArgs += @("-G", $Generator)
    if ($Generator -like "Visual Studio*") {
        $configureArgs += @("-A", $Arch)
    }
}

& cmake @configureArgs
if ($LASTEXITCODE -ne 0) {
    throw "CMake configure failed."
}

if (-not $ConfigureOnly) {
    & cmake --build $buildDirPath --config $Config
    if ($LASTEXITCODE -ne 0) {
        throw "CMake build failed."
    }
}
