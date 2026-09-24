/*
 * 异或法交换的致命前提：两个指针不能指向同一处。
 *
 * 编译运行：make run-alias   或   gcc -O2 -Wall -o test_alias test_alias.c && ./test_alias
 *
 * 实测（gcc 4.9.4 i686，-O0 与 -O2 输出逐字节一致）：
 *     swap_xor(&x, &x)  且 x = 5   →   x 变成 0
 *
 * 为什么：异或法交换的前提是 a、b 是两个互相独立的值。
 * 两个指针指向同一处时，第一行
 *
 *     *p = *p ^ *q;
 *
 * 就退化成 x = x ^ x，而任何数异或自己等于 0——值当场被清零，
 * 后面两行再怎么算也救不回来。
 *
 * 这不是"极端边界情况"：排序、去重、泛型容器里 swap(&v, &v) 都很常见。
 */
#include <stdio.h>

static void swap_xor(int *p, int *q) {
    *p = *p ^ *q;
    *q = *p ^ *q;
    *p = *p ^ *q;
}

int main(void) {
    int x = 5;
    printf("case 1: swap_xor(&x, &x)  with x = %d\n", x);
    swap_xor(&x, &x);
    printf("  after : x = %d\n", x);
    printf("  wiped to zero? %s\n", (x == 0) ? "YES" : "no");

    int a = 5, b = 3;
    printf("case 2 (control): a = %d, b = %d -> ", a, b);
    swap_xor(&a, &b);
    printf("a = %d, b = %d   swapped? %s\n", a, b, (a == 3 && b == 5) ? "YES" : "NO");

    return 0;
}
