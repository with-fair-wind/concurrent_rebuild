<#
.SYNOPSIS
  对所有 C/C++ 文件批量格式化并保存（显式使用指定的 clang-format.exe），排除 build 目录。

.DESCRIPTION
  1. 使用你提供的 clang-format.exe 路径
  2. 递归查找当前目录及子目录下的所有 .c/.cpp/.h/.hpp 文件
  3. 排除任何路径中包含 \build\ 或 /build/ 的文件
  4. 对其余文件执行 clang-format -i 原地格式化

.NOTES
  • 请确保下面这行中的路径与你本地安装路径一致：
      $clangFormatPath = 'C:\Users\kk\.vscode\extensions\ms-vscode.cpptools-1.26.3-win32-x64\LLVM\bin\clang-format.exe'
  • 将本脚本放在项目根目录后运行：
      PS> .\Format-AllCpp.ps1
#>

# 1. 切到脚本所在目录（项目根）
Set-Location (Split-Path -Parent $MyInvocation.MyCommand.Path)

# 2. 直接指定 clang-format.exe 路径
$clangFormatPath = 'C:\Users\kk\.vscode\extensions\ms-vscode.cpptools-1.26.3-win32-x64\LLVM\bin\clang-format.exe'

# 3. 检查 clang-format.exe 是否存在
if (-not (Test-Path $clangFormatPath)) {
    Write-Error "❗ 找不到 clang-format.exe：$clangFormatPath`n请检查路径是否正确。"
    exit 1
}

# 4. 查找所有 C/C++ 文件，排除 build 文件夹
$files = Get-ChildItem -Recurse -File -Include '*.c', '*.cpp', '*.h', '*.hpp' |
Where-Object { $_.FullName -notmatch '[\\/]+build[\\/]' }

if (-not $files) {
    Write-Host "⚠️ 未找到任何符合条件的 C/C++ 源文件。" -ForegroundColor Yellow
    exit 0
}

Write-Host "🔄 开始使用 `$clangFormatPath` 格式化以下文件（已排除 build 目录）：" -ForegroundColor Cyan
$files | ForEach-Object { Write-Host "  $_.FullName" }

# 5. 循环格式化
foreach ($file in $files) {
    & $clangFormatPath -i $file.FullName
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ 格式化成功：$($file.Name)"
    }
    else {
        Write-Host "❌ 格式化失败：$($file.Name)" -ForegroundColor Red
    }
}

Write-Host "`n🎉 格式化完成！" -ForegroundColor Green
