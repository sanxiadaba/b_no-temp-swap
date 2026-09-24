/*
 * 汇编对照用的样本。配合 tools/compare-asm.ps1 使用。
 *
 * 两组函数的差别**只有一处**：值是不是先落到互相独立的局部变量里。
 * 这一处差别决定了编译器敢不敢化简，也决定了四种写法是否归一。
 *
 *   f_*  int a = *p, b = *q; 再算，最后写回
 *        编译器能确定 a、b 互不影响 → 把加减法/异或法化简成一次寄存器交换
 *
 *   g_*  直接反复读写 *p / *q
 *        编译器不敢假设 p、q 不重叠（别名），必须每次重新载入 → 无法化简
 *
 * 加 noinline 是为了让每个函数都独立成段，方便逐条比对。
 * 不为了让汇编好看而改语义——这是能用 -S 直接复现的真实代码。
 */

#if defined(_MSC_VER)
#  define NOINLINE __declspec(noinline)
#else
#  define NOINLINE __attribute__((noinline))
#endif

/* ---- f 组：值是互相独立的局部变量 ---- */

NOINLINE void f_add(int *p, int *q) { int a = *p, b = *q; a = a + b; b = a - b; a = a - b; *p = a; *q = b; }
NOINLINE void f_mul(int *p, int *q) { int a = *p, b = *q; a = a * b; b = a / b; a = a / b; *p = a; *q = b; }
NOINLINE void f_xor(int *p, int *q) { int a = *p, b = *q; a = a ^ b; b = a ^ b; a = a ^ b; *p = a; *q = b; }
NOINLINE void f_tmp(int *p, int *q) { int a = *p, b = *q; int t = a; a = b; b = t; *p = a; *q = b; }

/* ---- g 组：直接反复读写指针，编译器无法排除别名 ---- */

NOINLINE void g_add(int *p, int *q) { *p = *p + *q; *q = *p - *q; *p = *p - *q; }
NOINLINE void g_xor(int *p, int *q) { *p = *p ^ *q; *q = *p ^ *q; *p = *p ^ *q; }
NOINLINE void g_tmp(int *p, int *q) { int t = *p; *p = *q; *q = t; }
