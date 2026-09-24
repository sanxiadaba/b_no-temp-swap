# 本机实测输出

原样粘贴，未做任何润色。重跑：

```powershell
$env:CC = 'C:\software\mingw\mingw32\bin\gcc.exe'; pwsh -File build.ps1
```

环境：

```text
gcc.exe (i686-posix-dwarf-rev0, Built by MinGW-W64 project) 4.9.4
Microsoft Windows NT 10.0.19045.0
```

---

```text
== 编译 swap ==
== 编译 test_overflow ==
== 编译 test_alias ==

== 1. 四种写法自检 ==
=== 两个独立的值：四种写法都应该成功 ===

写法           3,5            0,0           -7,7            1,-1      123456,-654321
中间变量     ok       ok       ok       ok       ok  
加减法       ok       ok       ok       ok       ok  
乘除法       ok     n/a       ok       ok     n/a  
异或法       ok       ok       ok       ok       ok  

  n/a = 该写法的前提不满足（乘除法要求 a、b 都非零且乘积不溢出）

=== 前提被破坏：两个指针指向同一处（x = 5, swap(&x, &x)）===

  中间变量  x = 5  ok   不受影响
  加减法    x = 0  ok   第一行退化成 5+5，第二行 10-10 归零
  乘除法    x = 1  ok   第一行退化成 5*5，第三行 25/25 得 1
  异或法    x = 0  ok   第一行退化成 5^5，异或自己是 0

失败 0 项

== 2. 整数溢出 vs 浮点误差 ==
int    before  2147483647 1
int    after   1 2147483647
double before  0.10000000000000001 0.20000000000000001
double after   0.20000000000000001 0.10000000000000003
b restored?    no

== 3. 同地址异或清零 ==
case 1: swap_xor(&x, &x)  with x = 5
  after : x = 0
  wiped to zero? YES
case 2 (control): a = 5, b = 3 -> a = 3, b = 5   swapped? YES

== 4. 汇编对照 ==
$ C:\software\mingw\mingw32\bin\gcc.exe -O2 -S -o build/compare.s src/compare_asm.c

--- f_add (9 条) ---
    pushl	%ebx
    movl	8(%esp), %edx
    movl	12(%esp), %eax
    movl	(%edx), %ecx
    movl	(%eax), %ebx
    movl	%ebx, (%edx)
    movl	%ecx, (%eax)
    popl	%ebx
    ret

--- f_mul (21 条) ---
    pushl	%edi
    pushl	%esi
    pushl	%ebx
    movl	16(%esp), %edi
    movl	20(%esp), %esi
    movl	(%edi), %ecx
    movl	(%esi), %ebx
    imull	%ebx, %ecx
    movl	%ecx, %eax
    cltd
    idivl	%ebx
    movl	%eax, %ebx
    movl	%ecx, %eax
    cltd
    idivl	%ebx
    movl	%eax, (%edi)
    movl	%ebx, (%esi)
    popl	%ebx
    popl	%esi
    popl	%edi
    ret

--- f_xor (9 条) ---
    pushl	%ebx
    movl	8(%esp), %edx
    movl	12(%esp), %eax
    movl	(%edx), %ecx
    movl	(%eax), %ebx
    movl	%ebx, (%edx)
    movl	%ecx, (%eax)
    popl	%ebx
    ret

--- f_tmp (9 条) ---
    pushl	%ebx
    movl	8(%esp), %edx
    movl	12(%esp), %eax
    movl	(%edx), %ecx
    movl	(%eax), %ebx
    movl	%ebx, (%edx)
    movl	%ecx, (%eax)
    popl	%ebx
    ret

--- g_add (9 条) ---
    movl	4(%esp), %edx
    movl	8(%esp), %ecx
    movl	(%ecx), %eax
    addl	(%edx), %eax
    movl	%eax, (%edx)
    subl	(%ecx), %eax
    movl	%eax, (%ecx)
    subl	%eax, (%edx)
    ret

--- g_xor (9 条) ---
    movl	4(%esp), %edx
    movl	8(%esp), %ecx
    movl	(%edx), %eax
    xorl	(%ecx), %eax
    movl	%eax, (%edx)
    xorl	(%ecx), %eax
    movl	%eax, (%ecx)
    xorl	%eax, (%edx)
    ret

--- g_tmp (9 条) ---
    pushl	%ebx
    movl	8(%esp), %edx
    movl	12(%esp), %eax
    movl	(%edx), %ecx
    movl	(%eax), %ebx
    movl	%ebx, (%edx)
    movl	%ecx, (%eax)
    popl	%ebx
    ret

=========== 逐字节比对 ===========
  f 组（值是互相独立的局部变量）
    f_add vs f_xor : SAME
    f_add vs f_tmp : SAME
    f_xor vs f_tmp : SAME
    f_add vs f_mul : DIFFERENT
  g 组（直接反复读写指针）
    g_add vs g_xor : DIFFERENT
    g_add vs g_tmp : DIFFERENT

条数：
  f_add    9
  f_mul   21
  f_xor    9
  f_tmp    9
  g_add    9
  g_xor    9
  g_tmp    9
```
