/* 유닛 테스트 — 외부 프레임워크 없이 표준 C만 쓴다.
 * 실행: make test-c
 */
#include <stdio.h>
#include <string.h>
#include "sort.h"

static int checks = 0;
static int failures = 0;

static void printArray(const char *label, const int a[], int n) {
    printf("      %s:", label);
    for (int i = 0; i < n; i++) {
        printf(" %d", a[i]);
    }
    printf("\n");
}

/* input을 정렬한 결과가 want와 같은지 본다. */
static void expectSorted(const char *name, int input[], const int want[], int n) {
    checks++;
    bubbleSort(input, n);
    if (n > 0 && memcmp(input, want, (size_t)n * sizeof(int)) != 0) {
        failures++;
        printf("FAIL  %s\n", name);
        printArray("got ", input, n);
        printArray("want", want, n);
        return;
    }
    printf("ok    %s\n", name);
}

int main(void) {
    {
        int a[] = {6, 8, 5, 9, 10, 1, 7, 2, 4, 3};
        const int want[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        expectSorted("섞인 배열", a, want, 10);
    }
    {
        int a[] = {1, 2, 3, 4, 5};
        const int want[] = {1, 2, 3, 4, 5};
        expectSorted("이미 정렬된 배열", a, want, 5);
    }
    {
        int a[] = {5, 4, 3, 2, 1};
        const int want[] = {1, 2, 3, 4, 5};
        expectSorted("역순 배열", a, want, 5);
    }
    {
        int a[] = {3, 1, 3, 1, 2};
        const int want[] = {1, 1, 2, 3, 3};
        expectSorted("중복이 있는 배열", a, want, 5);
    }
    {
        int a[] = {42};
        const int want[] = {42};
        expectSorted("원소 하나", a, want, 1);
    }
    {
        /* n = 0이면 배열을 건드리지 않는다. 초기화해 두어야 경고가 없다. */
        int a[1] = {0};
        const int want[1] = {0};
        expectSorted("빈 배열", a, want, 0);
    }

    printf("\n%d checks, %d failures\n", checks, failures);
    return failures == 0 ? 0 : 1;
}
