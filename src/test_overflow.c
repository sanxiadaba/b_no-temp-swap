/*
 * 加减法交换的两个隐患，实测。
 *
 * 编译运行：make run-overflow   或   gcc -O2 -Wall -o test_overflow test_overflow.c && ./test_overflow
 *
 * 结论（本机 gcc 4.9.4 i686 -O2）：
 *
 *   1. 整数溢出之后 **仍然换对了**。
 *      补码下加减互为逆运算，回绕之后照样抵消：INT_MAX,1 → 1,2147483647。
 *      这跟很多文章说的"溢出会导致交换失败"不一样。
 *      它依然是隐患——有符号溢出在 C 里是未定义行为，编译器有权做任何事
 *      （本仓库 tools/ 下的三变量轮换实验就是 UB 真的咬人的例子）——
 *      但它不会像传说中那样当场失败。
 *
 *   2. 浮点数才是真的换不回来：0.1 加 0.2 再减回去得到
 *      0.10000000000000003，而不是 0.10000000000000001。
 *      这不是编译器的 bug，是浮点表示本身的固有性质。
 */
#include <limits.h>
#include <stdio.h>

static void show_int(void) {
    int a = INT_MAX, b = 1;
    printf("int    before  %d %d\n", a, b);
    a = a + b; b = a - b; a = a - b;
    printf("int    after   %d %d\n", a, b);
}

static void show_double(void) {
    double a = 0.1, b = 0.2, original_a = a;
    printf("double before  %.17g %.17g\n", a, b);
    a = a + b; b = a - b; a = a - b;
    printf("double after   %.17g %.17g\n", a, b);

    /* 判定由程序自己算，不靠人眼去数 17 位小数 */
    printf("b restored?    %s\n", (b == original_a) ? "yes" : "no");
}

int main(void) {
    show_int();
    show_double();
    return 0;
}
