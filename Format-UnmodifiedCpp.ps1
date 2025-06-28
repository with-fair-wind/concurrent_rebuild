<#
.SYNOPSIS
  对所有 C/C++ 文件批量格式化并保存（显式使用指定的 clang-format.exe）。

.DESCRIPTION
  1. 从你提供的 clangd.exe 路径推导 clang-format.exe 路径
  2. 递归查找当前目录及子目录下的所有 .c/.cpp/.h/.hpp 文件
  3. 对每个文件执行 clang-format -i 原地格式化

.NOTES
  • 请确保以下两行中的路径与你本地一致：
      $clangdPath = 'c:\Users\kk\AppData\Roaming\Code\User\globalStorage\llvm-vs-code-extensions.vscode-clangd\install\19.1.2\clangd_19.1.2\bin\clangd.exe'
      $clangFormatExeName = 'clang-format.exe'
  • 在项目根目录执行： 
      PS> .\Format-AllCpp.ps1
#>

# 1. 脚本根目录设定
Set-Location (Split-Path -Parent $MyInvocation.MyCommand.Path)

# 2. 指定 clangd.exe 路径（你提供的路径）
$clangdPath = 'C:\Users\kk\.vscode\extensions\ms-vscode.cpptools-1.26.3-win32-x64\LLVM\bin\clang-format.exe'

# 3. 推导出同目录下的 clang-format.exe
$llvmBinDir = Split-Path $clangdPath
$clangFormatPath = Join-Path $llvmBinDir 'clang-format.exe'

# 4. 检查 clang-format 是否存在
if (-not (Test-Path $clangFormatPath)) {
    Write-Error "❗ 找不到 clang-format.exe：$clangFormatPath`n请确认该文件存在，或修改脚本中 \$clangFormatPath 为正确路径。"
    exit 1
}

# 5. 查找所有 C/C++ 源头文件
$files = Get-ChildItem -Recurse -File -Include '*.c', '*.cpp', '*.h', '*.hpp'

if (-not $files) {
    Write-Host "⚠️ 未找到任何 C/C++ 源文件。" -ForegroundColor Yellow
    exit 0
}

Write-Host "🔄 开始使用 `$clangFormatPath 格式化以下文件：" -ForegroundColor Cyan
$files | ForEach-Object { Write-Host "  $_.FullName" }

# 6. 对每个文件调用 clang-format 原地格式化
foreach ($file in $files) {
    & $clangFormatPath -i $file.FullName
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ 格式化成功：$($file.Name)"
    }
    else {
        Write-Host "❌ 格式化失败：$($file.Name)" -ForegroundColor Red
    }
}

Write-Host "`n🎉 全部文件格式化完成！" -ForegroundColor Green
