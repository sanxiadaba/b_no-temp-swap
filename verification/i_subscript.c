#include <stdio.h>

int main(void) {
    int a[4] = {10, 20, 30, 40};
    int i = 2;
    printf("a[i]=%d i[a]=%d\n", a[i], i[a]);
    printf("*(a+i)=%d *(i+a)=%d\n", *(a + i), *(i + a));
    return a[i] == 30 && i[a] == 30 ? 0 : 1;
}
