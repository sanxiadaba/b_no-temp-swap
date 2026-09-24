<#
.SYNOPSIS
    Windows 上的构建与实测入口（等价于 Makefile）。

.EXAMPLE
    pwsh -File build.ps1
    $env:CC = 'C:\software\mingw\mingw32\bin\gcc.exe'; pwsh -File build.ps1
#>
[CmdletBinding()]
param(
    [string]$Compiler = $(if ($env:CC) { $env:CC } else { 'gcc' }),
    [string]$Opt = '-O2'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $root 'build'
New-Item -ItemType Directory -Force -Path $build | Out-Null

$cflags = @($Opt, '-Wall', '-Wextra', '-std=c99')

function Build-One([string]$name) {
    Write-Host "== 编译 $name =="
    & $Compiler @cflags -o (Join-Path $build $name) (Join-Path $root "src/$name.c")
    if ($LASTEXITCODE -ne 0) { throw "编译 $name 失败（找不到 $Compiler？用 -Compiler 或环境变量 CC 指定）" }
}

foreach ($n in @('swap', 'test_overflow', 'test_alias')) { Build-One $n }

Write-Host ""
Write-Host "== 1. 四种写法自检 =="
& (Join-Path $build 'swap')

Write-Host ""
Write-Host "== 2. 整数溢出 vs 浮点误差 =="
& (Join-Path $build 'test_overflow')

Write-Host ""
Write-Host "== 3. 同地址异或清零 =="
& (Join-Path $build 'test_alias')

Write-Host ""
Write-Host "== 4. 汇编对照 =="
& pwsh -NoProfile -File (Join-Path $root 'tools/compare-asm.ps1') -Compiler $Compiler -Opt $Opt
