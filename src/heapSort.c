#include "sortctx.h"

/* root 아래를 최대 힙으로 다시 정리한다 (힙의 크기는 n) */
static void siftDown(SortCtx *c, size_t root, size_t n) {
    while (2 * root + 1 < n) {
        size_t child = 2 * root + 1;                                   /* 왼쪽 자식 */
        if (child + 1 < n && sortCompareAt(c, child, child + 1) < 0)
            child++;                                                   /* 더 큰 자식 */
        if (sortCompareAt(c, root, child) >= 0) return;                /* 이미 힙이다 */
        sortSwap(c, root, child);
        root = child;
    }
}

void heapSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (n < 2) return;
    if (!sortCtxInit(&c, base, size, cmp, stats)) return;
    sortDepth(&c, 1);

    for (size_t i = n / 2; i-- > 0;)          /* 1단계: 최대 힙 만들기, O(n) */
        siftDown(&c, i, n);
    for (size_t end = n - 1; end > 0; end--) {  /* 2단계: 최댓값을 뒤로 보내기, O(n log n) */
        sortSwap(&c, 0, end);
        siftDown(&c, 0, end);
    }
    sortCtxFree(&c);
}
