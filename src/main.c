#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bench.h"

/* 무엇을 잴지는 아래 상수 한 곳에만 적혀 있다 */
#define SHAPE_N 10000
static const size_t GROWTH_N[] = {1000, 2000, 4000, 8000, 16000, 32000};
#define GROWTH_COUNT (sizeof(GROWTH_N) / sizeof(GROWTH_N[0]))
#define SEED 20260929u

typedef struct Row {
    const char *section;
    const char *shape;
    size_t n;
    const SortAlgorithm *algo;
    BenchResult r;
} Row;

/* 결과를 표로 찍을지 CSV로 찍을지는 함수 포인터로 갈아 끼운다 */
typedef void (*RowSink)(const Row *row);

static void tableSink(const Row *w) {
    printf("%-14s %-9s %6lu %-14s %10.3f %14llu %14llu %4d %s\n",
           w->section, w->shape, (unsigned long)w->n, w->algo->name, w->r.ms,
           w->r.stats.compares, w->r.stats.moves, w->r.stats.maxDepth,
           w->r.sorted ? "" : "정렬 실패!");
}

static void csvSink(const Row *w) {
    printf("%s,%s,%lu,%s,%.4f,%llu,%llu,%d,%lu,%d\n",
           w->section, w->shape, (unsigned long)w->n, w->algo->name, w->r.ms,
           w->r.stats.compares, w->r.stats.moves, w->r.stats.maxDepth,
           (unsigned long)w->r.stats.extraBytes, w->r.stable);
}

static int repsFor(size_t n) { return n <= 2000 ? 10 : (n <= 8000 ? 3 : 1); }

static void runOne(RowSink sink, const char *section, InputShape shape, size_t n) {
    Record *input = makeInput(shape, n, SEED);
    if (input == NULL) return;
    for (size_t k = 0; k < SORT_ALGORITHM_COUNT; k++) {
        Row row;
        row.section = section;
        row.shape = SHAPE_NAMES[shape];
        row.n = n;
        row.algo = &SORT_ALGORITHMS[k];
        row.r = benchRun(row.algo, input, n, repsFor(n));
        sink(&row);
    }
    free(input);
}

/* 퀵 정렬의 피벗 전략 실험: 퀵 정렬만 피벗을 바꿔 가며 잰다 */
static void runPivot(void) {
    static const char *modeNames[] = {"last", "median3", "random"};
    const SortAlgorithm *quick = &SORT_ALGORITHMS[1];
    printf("shape,pivot,n,ms,compares,moves,maxDepth\n");
    for (int m = 0; m < 3; m++) {
        quickSortPivotMode = (QuickPivotMode)m;
        for (int s = 0; s < SHAPE_COUNT; s++) {
            Record *input = makeInput((InputShape)s, SHAPE_N, SEED);
            BenchResult r = benchRun(quick, input, SHAPE_N, 3);
            printf("%s,%s,%d,%.4f,%llu,%llu,%d\n", SHAPE_NAMES[s], modeNames[m], SHAPE_N,
                   r.ms, r.stats.compares, r.stats.moves, r.stats.maxDepth);
            free(input);
        }
    }
    quickSortPivotMode = QUICK_PIVOT_LAST;
}

int main(int argc, char **argv) {
    int csv = argc > 1 && strcmp(argv[1], "--csv") == 0;
    RowSink sink = csv ? csvSink : tableSink;

    if (argc > 1 && strcmp(argv[1], "--pivot") == 0) { runPivot(); return 0; }

    if (csv) printf("section,shape,n,algorithm,ms,compares,moves,maxDepth,extraBytes,stable\n");
    else     printf("%-14s %-9s %6s %-14s %10s %14s %14s %4s\n",
                    "section", "shape", "n", "algorithm", "ms", "compares", "moves", "depth");

    for (int s = 0; s < SHAPE_COUNT; s++) runOne(sink, "shapes", (InputShape)s, SHAPE_N);
    for (size_t i = 0; i < GROWTH_COUNT; i++) runOne(sink, "growth-random", SHAPE_RANDOM, GROWTH_N[i]);
    for (size_t i = 0; i < GROWTH_COUNT; i++) runOne(sink, "growth-sorted", SHAPE_SORTED, GROWTH_N[i]);
    return 0;
}
