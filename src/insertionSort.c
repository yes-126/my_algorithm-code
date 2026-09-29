#include "sortctx.h"

void insertionSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats) {
    SortCtx c;
    if (n < 2) return;
    if (!sortCtxInit(&c, base, size, cmp, stats)) return;
    sortDepth(&c, 1);

    for (size_t i = 1; i < n; i++) {
        if (sortCompareAt(&c, i - 1, i) <= 0) continue;    /* 이미 제자리면 건너뛴다 */

        sortMove(&c, c.tmp, sortElemAt(&c, i));            /* a[i]를 들어낸다 */
        size_t j = i;
        do {                                               /* a[j-1] > tmp 는 이미 확인했다 */
            sortMove(&c, sortElemAt(&c, j), sortElemAt(&c, j - 1));
            j--;
        } while (j > 0 && sortCompareTmp(&c, j - 1) < 0);  /* tmp < a[j-1]. '<=' 이면 불안정해진다 */
        sortMove(&c, sortElemAt(&c, j), c.tmp);            /* 빈자리에 놓는다 */
    }
    sortCtxFree(&c);
}
