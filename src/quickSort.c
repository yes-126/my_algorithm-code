#include <stdint.h>
#include "sortctx.h"

QuickPivotMode quickSortPivotMode = QUICK_PIVOT_LAST;

/* 무작위 피벗용 난수. 씨앗을 고정해 두어 언제나 같은 결과가 나온다 */
static uint32_t rngState;
static uint32_t rngNext(void) {
    uint32_t x = rngState;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return rngState = x;
}

/* 고른 피벗을 a[hi]로 옮겨 놓는다 */
static void choosePivot(SortCtx *c, size_t lo, size_t hi) {
    if (quickSortPivotMode == QUICK_PIVOT_MEDIAN3 && hi - lo >= 2) {
        size_t mid = lo + (hi - lo) / 2;
        if (sortCompareAt(c, mid, lo) < 0) sortSwap(c, mid, lo);
        if (sortCompareAt(c, hi, lo) < 0)  sortSwap(c, hi, lo);
        if (sortCompareAt(c, hi, mid) < 0) sortSwap(c, hi, mid);
        sortSwap(c, mid, hi);              /* 이제 a[mid]가 중앙값이다 */
    } else if (quickSortPivotMode == QUICK_PIVOT_RANDOM) {
        sortSwap(c, lo + rngNext() % (hi - lo + 1), hi);
    }
}

/* Lomuto 분할: a[hi]를 피벗으로 삼아 "피벗 이하"를 앞으로 모은다 */
static size_t partition(SortCtx *c, size_t lo, size_t hi) {
    choosePivot(c, lo, hi);
    size_t i = lo;
    for (size_t j = lo; j < hi; j++) {
        if (sortCompareAt(c, j, hi) <= 0) {
            sortSwap(c, i, j);
            i++;
        }
    }
    sortSwap(c, i, hi);
    return i;
}

/* 작은 쪽만 재귀, 큰 쪽은 반복문 -> 재귀 깊이가 어떤 입력에서도 O(log n) */
static void quickRec(SortCtx *c, size_t lo, size_t hi, int depth) {
    sortDepth(c, depth);
    while (lo < hi) {
        size_t p = partition(c, lo, hi);
        if (p - lo < hi - p) {
            if (p > lo) quickRec(c, lo, p - 1, depth + 1);
            lo = p + 1;
        } else {
            if (p < hi) quickRec(c, p + 1, hi, depth + 1);
            hi = p - 1;                    /* 이 분기에서는 p > lo >= 0 이라 안전하다 */
        }
    }
}

void quickSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (n < 2) return;
    if (!sortCtxInit(&c, base, size, cmp, stats)) return;
    rngState = 2463534242u;
    quickRec(&c, 0, n - 1, 1);
    sortCtxFree(&c);
}
