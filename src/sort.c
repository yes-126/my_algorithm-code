#include <stdlib.h>
#include <string.h>
#include "sortctx.h"

int sortCtxInit(SortCtx *c, void *base, size_t size, SortCompare cmp, SortStats *stats) {
    c->base = (char *)base;
    c->size = size;
    c->cmp = cmp;
    c->stats = stats;
    c->tmp = (char *)malloc(size);
    if (c->tmp == NULL) return 0;
    if (stats != NULL && stats->extraBytes < size) stats->extraBytes = size;
    return 1;
}

void sortCtxFree(SortCtx *c) {
    free(c->tmp);
    c->tmp = NULL;
}

void sortDepth(SortCtx *c, int depth) {
    if (c->stats != NULL && depth > c->stats->maxDepth) c->stats->maxDepth = depth;
}

char *sortElemAt(const SortCtx *c, size_t i) {
    return c->base + i * c->size;
}

int sortCompareAt(SortCtx *c, size_t i, size_t j) {
    if (c->stats != NULL) c->stats->compares++;
    return c->cmp(sortElemAt(c, i), sortElemAt(c, j));
}

int sortCompareTmp(SortCtx *c, size_t i) {
    if (c->stats != NULL) c->stats->compares++;
    return c->cmp(c->tmp, sortElemAt(c, i));
}

void sortMove(SortCtx *c, void *dst, const void *src) {
    if (c->stats != NULL) c->stats->moves++;
    memcpy(dst, src, c->size);
}

void sortSwap(SortCtx *c, size_t i, size_t j) {
    if (i == j) return;                       /* 같은 자리는 옮길 것이 없다 */
    sortMove(c, c->tmp, sortElemAt(c, i));
    sortMove(c, sortElemAt(c, i), sortElemAt(c, j));
    sortMove(c, sortElemAt(c, j), c->tmp);
}

/* 구현 표: 정렬을 하나 더 넣으려면 여기에 한 줄만 더하면 된다 */
const SortAlgorithm SORT_ALGORITHMS[] = {
    {"insertionSort", "O(n^2)",     "O(1)",     1, insertionSort},
    {"quickSort",     "O(n^2)",     "O(log n)", 0, quickSort},
    {"heapSort",      "O(n log n)", "O(1)",     0, heapSort},
};
const size_t SORT_ALGORITHM_COUNT = sizeof(SORT_ALGORITHMS) / sizeof(SORT_ALGORITHMS[0]);
