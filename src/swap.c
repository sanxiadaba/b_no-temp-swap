#include "swap.h"

#include <stdio.h>

void swap_tmp(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void swap_add(int *a, int *b) {
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}

void swap_mul(int *a, int *b) {
    *a = *a * *b;
    *b = *a / *b;
    *a = *a / *b;
}

void swap_xor(int *a, int *b) {
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}

/* ------------------------------------------------------------------ */

typedef void (*swap_fn)(int *, int *);

/*
 * 每种写法的适用前提。
 *
 * 前三种写法里只有乘除法**会在正常输入下直接崩**：
 *
 *     a = a * b;      a*b
 *     b = a / b;      需要 b != 0
 *     a = a / b;      需要 a != 0
 *
 * 而且 a*b 还必须不溢出。这正是"乘除法把缺点放大了"的具体含义
 * ——它比加减法多一种失效模式（除零），不是笼统的"更危险"。
 */
typedef int (*applicable_fn)(int, int);

static int always_ok(int a, int b) { (void)a; (void)b; return 1; }

static int mul_ok(int a, int b) {
    if (a == 0 || b == 0) return 0;                       /* 会除零 */
    long long product = (long long)a * (long long)b;
    return product >= -2147483647LL - 1 && product <= 2147483647LL;
}

static const struct {
    const char *name;
    swap_fn fn;
    applicable_fn applicable;
} CASES[] = {
    { "中间变量", swap_tmp, always_ok },
    { "加减法  ", swap_add, always_ok },
    { "乘除法  ", swap_mul, mul_ok },
    { "异或法  ", swap_xor, always_ok },
};
#define N_CASES ((int)(sizeof(CASES) / sizeof(CASES[0])))

static const int VALUE_PAIRS[][2] = {
    { 3, 5 },
    { 0, 0 },
    { -7, 7 },
    { 1, -1 },
    { 123456, -654321 },
};
#define N_PAIRS ((int)(sizeof(VALUE_PAIRS) / sizeof(VALUE_PAIRS[0])))

int swap_selftest(void) {
    int failures = 0;

    printf("=== 两个独立的值：四种写法都应该成功 ===\n\n");
    printf("%-10s", "写法");
    for (int i = 0; i < N_PAIRS; i += 1) {
        printf("  %6d,%-6d", VALUE_PAIRS[i][0], VALUE_PAIRS[i][1]);
    }
    printf("\n");

    for (int c = 0; c < N_CASES; c += 1) {
        printf("%-10s", CASES[c].name);
        for (int i = 0; i < N_PAIRS; i += 1) {
            const int a0 = VALUE_PAIRS[i][0], b0 = VALUE_PAIRS[i][1];

            /* 前提不满足就跳过。让它跑下去会直接崩，而不是"算错" */
            if (!CASES[c].applicable(a0, b0)) {
                printf("   n/a  ");
                continue;
            }

            int a = a0, b = b0;
            CASES[c].fn(&a, &b);
            const int ok = (a == b0 && b == a0);
            if (!ok) failures += 1;
            printf("  %s", ok ? "   ok  " : " FAIL  ");
        }
        printf("\n");
    }
    printf("\n  n/a = 该写法的前提不满足（乘除法要求 a、b 都非零且乘积不溢出）\n");

    /*
     * 前提被破坏：两个指针指向同一个对象。
     *
     * 这不是"极端边界情况"——把这段代码写成函数之后，
     * swap(&v, &v) 在排序、去重、泛型容器里都很常见。
     *
     * ⚠️ 被破坏的**不只是异或法**：除中间变量外，三种写法全都会算错，
     * 而且错得各不相同。所以"a 与 b 必须互相独立"是这个格式的共同前提，
     * 不是异或法独有的注意事项。
     */
    printf("\n=== 前提被破坏：两个指针指向同一处（x = 5, swap(&x, &x)）===\n\n");

    /* 每种写法在同地址下的实际结果，实测得来。中间变量是唯一安全的 */
    static const int ALIAS_EXPECTED[] = { 5, 0, 1, 0 };
    static const char *ALIAS_NOTE[] = {
        "不受影响",
        "第一行退化成 5+5，第二行 10-10 归零",
        "第一行退化成 5*5，第三行 25/25 得 1",
        "第一行退化成 5^5，异或自己是 0",
    };

    for (int c = 0; c < N_CASES; c += 1) {
        int x = 5;
        CASES[c].fn(&x, &x);
        const int ok = (x == ALIAS_EXPECTED[c]);
        if (!ok) failures += 1;
        printf("  %s  x = %d  %-4s %s\n",
               CASES[c].name, x, ok ? "ok" : "??", ALIAS_NOTE[c]);
    }

    printf("\n失败 %d 项\n", failures);
    return failures;
}

int main(void) {
    return swap_selftest() == 0 ? 0 : 1;
}
