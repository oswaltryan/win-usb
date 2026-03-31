function Get-DefaultCMakeGenerator {
    $helpText = (& cmake --help) -join "`n"
    $preferredGenerators = @(
        "Visual Studio 18 2026",
        "Visual Studio 17 2022",
        "Visual Studio 16 2019"
    )

    foreach ($candidate in $preferredGenerators) {
        if ($helpText -match [Regex]::Escape($candidate)) {
            return $candidate
        }
    }
    return ""
}

function Resolve-BuildDirPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$RepoRoot,
        [Parameter(Mandatory = $true)]
        [string]$BuildDir
    )

    $repoRootPath = [System.IO.Path]::GetFullPath($RepoRoot)
    $buildDirPath = if ([System.IO.Path]::IsPathRooted($BuildDir)) {
        [System.IO.Path]::GetFullPath($BuildDir)
    } else {
        [System.IO.Path]::GetFullPath((Join-Path $repoRootPath $BuildDir))
    }

    if (-not $buildDirPath.StartsWith($repoRootPath, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "BuildDir must resolve inside the repository root."
    }

    return $buildDirPath
}
