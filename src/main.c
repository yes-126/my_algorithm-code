/* 실행: make run-c */
#include <stdio.h>
#include "sort.h"

int main(void) {
    int a[] = {6, 8, 5, 9, 10, 1, 7, 2, 4, 3};
    int n = (int)(sizeof(a) / sizeof(a[0]));

    bubbleSort(a, n);

    printf("sorted:");
    for (int i = 0; i < n; i++) {
        printf(" %d", a[i]);
    }
    printf("\n");
    return 0;
}
