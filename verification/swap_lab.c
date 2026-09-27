#include <stdio.h>

static int swap_tmp(unsigned *a, unsigned *b) {
    if (a == NULL || b == NULL || a == b) return 0;
    unsigned tmp = *a;
    *a = *b;
    *b = tmp;
    return 1;
}

static int swap_xor(unsigned *a, unsigned *b) {
    if (a == NULL || b == NULL || a == b) return 0;
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
    return 1;
}

static void swap_xor_unchecked(unsigned *a, unsigned *b) {
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

int main(void) {
    unsigned a = 3, b = 5;
    if (!swap_tmp(&a, &b) || a != 5 || b != 3) return 1;
    printf("tmp, distinct objects: a=%u b=%u\n", a, b);

    a = 3; b = 5;
    if (!swap_xor(&a, &b) || a != 5 || b != 3) return 2;
    printf("xor, distinct objects: a=%u b=%u\n", a, b);

    unsigned x = 3;
    swap_xor_unchecked(&x, &x);
    printf("xor, same object: x=%u\n", x);

    x = 3;
    const int accepted = swap_xor(&x, &x);
    printf("guard, same object: accepted=%d x=%u\n", accepted, x);
    return accepted == 0 && x == 3 ? 0 : 3;
}
