<#
.SYNOPSIS
    生成四种交换写法的汇编，去掉噪音，再逐条比对它们是否相同。

.DESCRIPTION
    这是视频里"编译器把它们归一了"那句话的证据来源。

    ⚠️ 结论有适用范围，脚本会把两组都打出来：

      f_* 值是互相独立的局部变量 —— 编译器敢化简
          加减法 / 异或法 / 中间变量 生成逐字节相同的汇编，乘除法例外

      g_* 直接反复读写指针 —— 编译器不敢排除别名
          四种各不相同

    差别就在别名：这正是视频里"两个指针指向同一处"那一节讲的问题。

.EXAMPLE
    pwsh -File tools/compare-asm.ps1
#>
[CmdletBinding()]
param(
    [string]$Compiler = $(if ($env:CC) { $env:CC } else { 'gcc' }),
    # 优化级别。实测 -O1 / -O2 / -O3 在本机产出逐字节相同的汇编
    [string]$Opt = '-O2'
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$out = Join-Path $root 'build'
New-Item -ItemType Directory -Force -Path $out | Out-Null

$asm = Join-Path $out 'compare.s'
Write-Host "$ $Compiler $Opt -S -o build/compare.s src/compare_asm.c"
& $Compiler $Opt -S -o $asm (Join-Path $root 'src/compare_asm.c')
if ($LASTEXITCODE -ne 0) { throw "编译失败（找不到 $Compiler？用 -Compiler 或环境变量 CC 指定）" }

# 只保留指令本身：去掉注释、伪指令、以及 LFB0 这类纯标签行
$ins = [ordered]@{}
$cur = $null
foreach ($l in (Get-Content -LiteralPath $asm)) {
    # MinGW 会在符号前加下划线，两种都认
    if ($l -match '^_?((?:f|g)_\w+):') { $cur = $Matches[1]; $ins[$cur] = @(); continue }
    if ($l -match '^(L[A-Z]+\d*|\.\w+):') { continue }
    if ($cur -and $l -match '^\s+([a-z][a-z0-9]*)(\s+.*)?$') {
        $ins[$cur] += ($Matches[1] + $Matches[2]).TrimEnd()
    }
}

foreach ($k in $ins.Keys) {
    Write-Host ""
    Write-Host "--- $k ($($ins[$k].Count) 条) ---"
    $ins[$k] | ForEach-Object { Write-Host "    $_" }
}

function Compare-Pair($a, $b) {
    if (-not $ins.Contains($a) -or -not $ins.Contains($b)) { return 'n/a' }
    if (($ins[$a] -join ',') -eq ($ins[$b] -join ',')) { 'SAME' } else { 'DIFFERENT' }
}

Write-Host ""
Write-Host "=========== 逐字节比对 ==========="
Write-Host "  f 组（值是互相独立的局部变量）"
Write-Host "    f_add vs f_xor : $(Compare-Pair 'f_add' 'f_xor')"
Write-Host "    f_add vs f_tmp : $(Compare-Pair 'f_add' 'f_tmp')"
Write-Host "    f_xor vs f_tmp : $(Compare-Pair 'f_xor' 'f_tmp')"
Write-Host "    f_add vs f_mul : $(Compare-Pair 'f_add' 'f_mul')"
Write-Host "  g 组（直接反复读写指针）"
Write-Host "    g_add vs g_xor : $(Compare-Pair 'g_add' 'g_xor')"
Write-Host "    g_add vs g_tmp : $(Compare-Pair 'g_add' 'g_tmp')"
Write-Host ""
Write-Host "条数："
foreach ($n in @('f_add', 'f_mul', 'f_xor', 'f_tmp', 'g_add', 'g_xor', 'g_tmp')) {
    if ($ins.Contains($n)) { Write-Host ("  {0,-6} {1,3}" -f $n, $ins[$n].Count) }
}
