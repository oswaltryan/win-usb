function Resolve-ToolPath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ToolName,
        [string[]]$CandidatePaths = @()
    )

    $cmd = Get-Command $ToolName -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Source
    }

    foreach ($path in $CandidatePaths) {
        if ($path -and (Test-Path $path)) {
            return $path
        }
    }

    return $null
}

function Get-CmakelintPath {
    $roamingScripts = Get-ChildItem `
        -Path (Join-Path $env:APPDATA "Python") `
        -Filter "cmakelint.exe" `
        -Recurse `
        -ErrorAction SilentlyContinue |
        Select-Object -First 1 -ExpandProperty FullName

    return Resolve-ToolPath -ToolName "cmakelint" -CandidatePaths @($roamingScripts)
}

function Get-ClangFormatPath {
    return Resolve-ToolPath -ToolName "clang-format" -CandidatePaths @(
        "C:\Program Files\LLVM\bin\clang-format.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\Llvm\x64\bin\clang-format.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\17\BuildTools\VC\Tools\Llvm\x64\bin\clang-format.exe"
    )
}

function Get-ClangTidyPath {
    return Resolve-ToolPath -ToolName "clang-tidy" -CandidatePaths @(
        "C:\Program Files\LLVM\bin\clang-tidy.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Tools\Llvm\x64\bin\clang-tidy.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\17\BuildTools\VC\Tools\Llvm\x64\bin\clang-tidy.exe"
    )
}

function Get-CppcheckPath {
    return Resolve-ToolPath -ToolName "cppcheck" -CandidatePaths @(
        "C:\Program Files\Cppcheck\cppcheck.exe",
        "C:\Program Files (x86)\Cppcheck\cppcheck.exe"
    )
}
