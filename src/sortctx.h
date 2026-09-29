#ifndef SORTCTX_H
#define SORTCTX_H

/* 구현끼리만 쓰는 헤더. main.c 같은 바깥은 sort.h만 본다 */
#include "sort.h"

typedef struct SortCtx {
    char *base;        /* 배열의 첫 바이트 */
    size_t size;       /* 원소 한 개의 바이트 수 */
    SortCompare cmp;
    SortStats *stats;
    char *tmp;         /* 원소 한 칸. 추가로 잡는 메모리는 이것뿐이다 */
} SortCtx;

int   sortCtxInit(SortCtx *c, void *base, size_t size, SortCompare cmp, SortStats *stats);
void  sortCtxFree(SortCtx *c);
void  sortDepth(SortCtx *c, int depth);

char *sortElemAt(const SortCtx *c, size_t i);
int   sortCompareAt(SortCtx *c, size_t i, size_t j);    /* cmp(a[i], a[j]) */
int   sortCompareTmp(SortCtx *c, size_t i);             /* cmp(tmp, a[i]) */
void  sortMove(SortCtx *c, void *dst, const void *src); /* 이동 1회 */
void  sortSwap(SortCtx *c, size_t i, size_t j);         /* 이동 3회 (tmp 경유) */

#endif
