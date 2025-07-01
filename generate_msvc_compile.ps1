<#
.SYNOPSIS
  Generate compile_commands.json in the build directory.

.DESCRIPTION
  1. Create the build folder if it does not exist.
  2. Locate clang-build.ps1.
  3. Clear old kJsonCompilationDbPath variables.
  4. Switch to build directory and invoke clang-build.ps1 to generate JSON.
#>

param(
    [string]$ProjectRoot = (Split-Path -Parent $MyInvocation.MyCommand.Definition),
    [string]$BuildDir = "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)\build",
    [string]$Config = ""
)

Write-Host "PowerShell version:" $PSVersionTable.PSVersion
Write-Host "Current script path:" $PSCommandPath
Write-Host "Current PowerShell process path:" (Get-Process -Id $PID).Path
Write-Host "Project root: $ProjectRoot"
Write-Host "Build dir:    $BuildDir"

# 1) Create build directory
if (-not (Test-Path $BuildDir)) {
    Write-Host "> Creating directory $BuildDir"
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# 2) Locate clang-build.ps1
Write-Host "> Locating clang-build.ps1..."
$clangBuild = Get-ChildItem `
    -Path "$env:LOCALAPPDATA\Microsoft\VisualStudio" `
    -Filter "clang-build.ps1" -Recurse -ErrorAction SilentlyContinue |
Select-Object -First 1

if (-not $clangBuild) {
    Write-Error "clang-build.ps1 not found. Please install Clang Power Tools first."
    exit 1
}
Write-Host "Found script: $($clangBuild.FullName)"

# 3) Switch to build directory and execute (JSON will be output here)
Push-Location $BuildDir
try {
    Write-Host "> Clearing old kJsonCompilationDbPath variables..."
    Remove-Variable -Name kJsonCompilationDbPath -Scope Script -Force -ErrorAction SilentlyContinue
    Remove-Variable -Name kJsonCompilationDbPath -Scope Global -Force -ErrorAction SilentlyContinue

    Write-Host "> Generating compile_commands.json..."
    & $clangBuild.FullName `
        -dir $ProjectRoot `
        -active-config "$Config|x64" `
        -export-jsondb `
        -parallel

    $out = Join-Path $ProjectRoot 'compile_commands.json'
    if (Test-Path $out) {
        Move-Item -Force $out $BuildDir
        Write-Host "> Moved compile_commands.json to $BuildDir"
    }
    else {
        Write-Warning "Operation completed, but compile_commands.json was not found in $ProjectRoot"
    }
}
finally {
    Pop-Location
}