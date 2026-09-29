#ifndef BENCH_H
#define BENCH_H

#include <stddef.h>
#include <stdint.h>
#include "sort.h"

/* 안정성을 재기 위한 원소: key로 정렬하고, tag에는 입력에서의 순서를 새겨 둔다 */
typedef struct Record {
    int key;   /* 정렬 기준 */
    int tag;   /* 입력에서의 순서 */
} Record;

typedef enum {
    SHAPE_RANDOM,     /* 무작위 순열 */
    SHAPE_SORTED,     /* 이미 정렬됨 */
    SHAPE_REVERSED,   /* 역순 */
    SHAPE_NEARLY,     /* 정렬 상태에서 1%만 무작위 교환 */
    SHAPE_DUPS,       /* 값이 16종류뿐 (중복 많음) */
    SHAPE_COUNT
} InputShape;

extern const char *SHAPE_NAMES[SHAPE_COUNT];

typedef struct BenchResult {
    double ms;          /* reps회 평균 시간 */
    SortStats stats;    /* 마지막 회차의 카운터 (입력이 고정이라 매번 같다) */
    int sorted;         /* key 기준으로 오름차순인가 */
    int stable;         /* 같은 key끼리 tag가 오름차순인가 (실측) */
} BenchResult;

int     recordCompare(const void *a, const void *b);      /* key만 본다 */
Record *makeInput(InputShape shape, size_t n, uint32_t seed);
int     isSortedByKey(const Record *a, size_t n);
int     isStable(const Record *a, size_t n);
BenchResult benchRun(const SortAlgorithm *algo, const Record *input, size_t n, int reps);

#endif
