<#
.SYNOPSIS
  在 build 目录下生成 compile_commands.json。

.DESCRIPTION
  1. 创建 build 文件夹（若不存在）。
  2. 查找 clang-build.ps1。
  3. 清除旧的 kJsonCompilationDbPath 变量。
  4. 切到 build，调用 clang-build.ps1 生成 JSON。
#>

param(
    [string]$ProjectRoot = (Split-Path -Parent $MyInvocation.MyCommand.Definition),
    [string]$BuildDir = "$(Split-Path -Parent $MyInvocation.MyCommand.Definition)\build",
    [string]$Config = ""
)

Write-Host "Project root: $ProjectRoot"
Write-Host "Build dir:    $BuildDir"

# 1) 创建 build 目录
if (-not (Test-Path $BuildDir)) {
    Write-Host "> 创建目录 $BuildDir"
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# 2) 查找 clang-build.ps1
Write-Host "> 查找 clang-build.ps1…"
$clangBuild = Get-ChildItem `
    -Path "$env:LOCALAPPDATA\Microsoft\VisualStudio" `
    -Filter "clang-build.ps1" -Recurse -ErrorAction SilentlyContinue |
Select-Object -First 1

if (-not $clangBuild) {
    Write-Error "未找到 clang-build.ps1，请先安装 Clang Power Tools。"
    exit 1
}
Write-Host "找到脚本： $($clangBuild.FullName)"

# 3) 切到 build 目录再执行（JSON 直接输出到这里）
Push-Location $BuildDir
try {
    Write-Host "> 清除旧的 kJsonCompilationDbPath 变量…"
    Remove-Variable -Name kJsonCompilationDbPath -Scope Script -Force -ErrorAction SilentlyContinue
    Remove-Variable -Name kJsonCompilationDbPath -Scope Global -Force -ErrorAction SilentlyContinue

    Write-Host "> 生成 compile_commands.json…"
    & $clangBuild.FullName `
        -dir $ProjectRoot `
        -active-config "$Config|x64" `
        -export-jsondb `
        -parallel

    $out = Join-Path $ProjectRoot 'compile_commands.json'
    if (Test-Path $out) {
        Move-Item -Force $out $BuildDir
        Write-Host "✔ 已移动 compile_commands.json 到 $BuildDir"
    }
    else {
        Write-Warning "操作完成，但未在 $ProjectRoot 找到 compile_commands.json"
    }
}
finally {
    Pop-Location
}
