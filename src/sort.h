#ifndef SORT_H
#define SORT_H

#include <stddef.h>

/* qsort와 같은 규약: a<b 이면 음수, a==b 이면 0, a>b 이면 양수 */
typedef int (*SortCompare)(const void *a, const void *b);

/* 정렬이 하는 일을 세는 카운터. NULL을 넘기면 측정하지 않는다 */
typedef struct SortStats {
    unsigned long long compares;   /* 원소 비교 횟수 */
    unsigned long long moves;      /* 원소 이동 횟수 (교환 1번 = 이동 3번) */
    size_t extraBytes;             /* 입력 배열 밖에 잡은 최대 바이트 */
    int maxDepth;                  /* 실제로 들어간 최대 재귀 깊이 */
} SortStats;

/* 정렬 하나를 설명하는 구조체. 함수 포인터를 담은 "인터페이스" 역할 */
typedef struct SortAlgorithm {
    const char *name;
    const char *timeComplexity;    /* 최악 시간 */
    const char *spaceComplexity;   /* 추가 메모리 */
    int stable;                    /* 안정 정렬이라고 "주장"하는 값 */
    void (*sort)(void *base, size_t n, size_t size,
                 SortCompare cmp, SortStats *stats);
} SortAlgorithm;

extern const SortAlgorithm SORT_ALGORITHMS[];   /* 구현 목록 */
extern const size_t SORT_ALGORITHM_COUNT;

void insertionSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);
void quickSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);
void heapSort(void *base, size_t n, size_t size, SortCompare cmp, SortStats *stats);

/* 퀵 정렬의 피벗 고르는 방법. 실험에서 바꿔 끼울 수 있도록 const가 아니다 */
typedef enum {
    QUICK_PIVOT_LAST = 0,      /* 구간의 마지막 원소 (기본) */
    QUICK_PIVOT_MEDIAN3 = 1,   /* 처음 · 가운데 · 마지막의 중앙값 */
    QUICK_PIVOT_RANDOM = 2     /* 무작위 (고정 씨앗) */
} QuickPivotMode;
extern QuickPivotMode quickSortPivotMode;

#endif
