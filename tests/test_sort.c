#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bench.h"

static int checks = 0, failures = 0;

#define CHECK(cond, ...)                                              \
    do {                                                              \
        checks++;                                                     \
        if (!(cond)) {                                                \
            failures++;                                               \
            printf("FAIL %s:%d  ", __FILE__, __LINE__);               \
            printf(__VA_ARGS__);                                      \
            printf("\n");                                             \
        }                                                             \
    } while (0)

static int intCompare(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

/* 표를 훑는다: 정렬을 하나 더 넣으면 그 순간부터 같은 검사를 받는다 */
#define FOR_EACH_ALGO(a) \
    for (const SortAlgorithm *a = SORT_ALGORITHMS; a < SORT_ALGORITHMS + SORT_ALGORITHM_COUNT; a++)

static void testBasicCases(void) {
    static const int cases[][8] = {
        {5, 2, 4, 1, 3, 8, 7, 6},   /* 섞임 */
        {1, 2, 3, 4, 5, 6, 7, 8},   /* 정렬됨 */
        {8, 7, 6, 5, 4, 3, 2, 1},   /* 역순 */
        {3, 1, 3, 1, 2, 2, 3, 1},   /* 중복 */
        {7, 7, 7, 7, 7, 7, 7, 7},   /* 모두 같은 값 */
        {42, 0, 0, 0, 0, 0, 0, 0},  /* 원소 1개 (n=1) */
        {9, 9, 9, 9, 9, 9, 9, 9},   /* 빈 배열 (n=0) */
    };
    static const size_t sizes[] = {8, 8, 8, 8, 8, 1, 0};
    FOR_EACH_ALGO(algo) {
        for (size_t c = 0; c < sizeof(sizes) / sizeof(sizes[0]); c++) {
            int got[8], want[8];
            memcpy(got, cases[c], sizeof got);
            memcpy(want, cases[c], sizeof want);
            qsort(want, sizes[c], sizeof(int), intCompare);
            algo->sort(got, sizes[c], sizeof(int), intCompare, NULL);
            CHECK(memcmp(got, want, sizeof got) == 0, "%s 기본 케이스 %zu", algo->name, c);
        }
    }
}

static void testAgainstQsort(void) {
    enum { N = 500 };
    FOR_EACH_ALGO(algo) {
        int got[N], want[N];
        uint32_t s = 12345;
        for (int i = 0; i < N; i++) {
            s ^= s << 13; s ^= s >> 17; s ^= s << 5;
            got[i] = want[i] = (int)(s % 1000);
        }
        qsort(want, N, sizeof(int), intCompare);
        algo->sort(got, N, sizeof(int), intCompare, NULL);
        CHECK(memcmp(got, want, sizeof got) == 0, "%s 는 qsort 결과와 같아야 한다", algo->name);
    }
}

static void testSizeSweep(void) {
    FOR_EACH_ALGO(algo) {
        for (size_t n = 0; n <= 200; n++) {
            for (int shape = SHAPE_RANDOM; shape <= SHAPE_DUPS; shape += SHAPE_DUPS - SHAPE_RANDOM) {
                Record *in = makeInput((InputShape)shape, n, 777);
                long long tagSum = 0, want = (long long)n * ((long long)n - 1) / 2;
                algo->sort(in, n, sizeof(Record), recordCompare, NULL);
                for (size_t i = 0; i < n; i++) tagSum += in[i].tag;
                CHECK(isSortedByKey(in, n) && tagSum == want,
                      "%s n=%zu shape=%s", algo->name, n, SHAPE_NAMES[shape]);
                free(in);
            }
        }
    }
}

static void testStability(void) {
    FOR_EACH_ALGO(algo) {
        int everUnstable = 0;
        for (int shape = 0; shape < SHAPE_COUNT; shape++) {
            Record *in = makeInput((InputShape)shape, 500, 4242);
            BenchResult r = benchRun(algo, in, 500, 1);
            if (algo->stable) CHECK(r.stable, "%s 는 stable=1 이라고 했는데 %s 에서 순서가 깨졌다",
                                    algo->name, SHAPE_NAMES[shape]);
            if (!r.stable) everUnstable = 1;
            free(in);
        }
        if (!algo->stable)
            CHECK(everUnstable, "%s 는 stable=0 이라고 했는데 한 번도 깨지지 않았다", algo->name);
    }
}

/* 보고서 2.3절의 3원소 예: (2,0) (2,1) (1,2) 를 key로 정렬한다 */
static void testTinyStabilityExample(void) {
    const Record in[3] = {{2, 0}, {2, 1}, {1, 2}};
    Record a[3];

    memcpy(a, in, sizeof a);
    insertionSort(a, 3, sizeof(Record), recordCompare, NULL);
    CHECK(a[0].tag == 2 && a[1].tag == 0 && a[2].tag == 1, "삽입 정렬은 같은 key의 순서를 지킨다");

    memcpy(a, in, sizeof a);
    heapSort(a, 3, sizeof(Record), recordCompare, NULL);
    CHECK(a[0].tag == 2 && a[1].tag == 1 && a[2].tag == 0, "힙 정렬은 이 예에서 같은 key의 순서를 뒤집는다");
}

static void testPivotModes(void) {
    const SortAlgorithm *quick = &SORT_ALGORITHMS[1];
    for (int mode = QUICK_PIVOT_LAST; mode <= QUICK_PIVOT_RANDOM; mode++) {
        quickSortPivotMode = (QuickPivotMode)mode;
        for (int shape = 0; shape < SHAPE_COUNT; shape++) {
            Record *in = makeInput((InputShape)shape, 1000, 99);
            BenchResult r = benchRun(quick, in, 1000, 1);
            CHECK(r.sorted, "quickSort pivot=%d shape=%s", mode, SHAPE_NAMES[shape]);
            free(in);
        }
    }
    quickSortPivotMode = QUICK_PIVOT_LAST;
}

static void testBenchTools(void) {
    enum { N = 1000 };
    Record *a;
    long long sum = 0;

    a = makeInput(SHAPE_SORTED, N, 1);
    CHECK(isSortedByKey(a, N), "sorted 입력이 정렬되어 있지 않다");
    free(a);

    a = makeInput(SHAPE_REVERSED, N, 1);
    CHECK(a[0].key == N - 1 && a[N - 1].key == 0 && !isSortedByKey(a, N), "reversed 입력 모양");
    free(a);

    a = makeInput(SHAPE_RANDOM, N, 1);
    for (int i = 0; i < N; i++) sum += a[i].key;
    CHECK(sum == (long long)N * (N - 1) / 2 && !isSortedByKey(a, N), "random 입력은 순열이어야 한다");
    free(a);

    a = makeInput(SHAPE_DUPS, N, 1);
    for (int i = 0; i < N; i++) CHECK(a[i].key >= 0 && a[i].key < 16, "dups 입력의 값 범위");
    free(a);

    Record bad[2] = {{1, 1}, {1, 0}}, good[2] = {{1, 0}, {1, 1}};
    CHECK(!isStable(bad, 2) && isStable(good, 2), "안정성 판정기가 위반을 잡아야 한다");
}

static SortStats runStats(const SortAlgorithm *algo, InputShape shape, size_t n) {
    Record *in = makeInput(shape, n, 5);
    SortStats st;
    memset(&st, 0, sizeof st);
    algo->sort(in, n, sizeof(Record), recordCompare, &st);
    free(in);
    return st;
}

static void testCounters(void) {
    const size_t n = 1024;
    const unsigned long long full = (unsigned long long)n * (n - 1) / 2;
    SortStats st;

    st = runStats(&SORT_ALGORITHMS[0], SHAPE_SORTED, n);
    CHECK(st.compares == n - 1 && st.moves == 0, "삽입 정렬: 정렬된 입력은 비교 n-1, 이동 0");
    st = runStats(&SORT_ALGORITHMS[0], SHAPE_REVERSED, n);
    CHECK(st.compares == full, "삽입 정렬: 역순의 비교는 n(n-1)/2");

    quickSortPivotMode = QUICK_PIVOT_LAST;
    st = runStats(&SORT_ALGORITHMS[1], SHAPE_SORTED, n);
    CHECK(st.compares == full && st.maxDepth == 1, "퀵 정렬(마지막 피벗): 정렬된 입력은 n(n-1)/2, 깊이 1");
    quickSortPivotMode = QUICK_PIVOT_MEDIAN3;
    st = runStats(&SORT_ALGORITHMS[1], SHAPE_SORTED, n);
    CHECK(st.compares < 20 * n, "퀵 정렬(중앙값 피벗): 정렬된 입력에서 O(n log n)");
    quickSortPivotMode = QUICK_PIVOT_LAST;

    st = runStats(&SORT_ALGORITHMS[2], SHAPE_SORTED, n);
    CHECK(st.compares <= 2 * n * 10, "힙 정렬: 비교는 2 n log2 n 이하");

    FOR_EACH_ALGO(algo) {
        st = runStats(algo, SHAPE_RANDOM, n);
        CHECK(st.extraBytes == sizeof(Record), "%s 의 추가 메모리는 원소 한 칸", algo->name);
    }
    st = runStats(&SORT_ALGORITHMS[1], SHAPE_RANDOM, 100000);
    CHECK(st.maxDepth <= 40, "퀵 정렬 재귀 깊이는 log n 수준이어야 한다 (실측 %d)", st.maxDepth);
}

int main(void) {
    testBasicCases();
    testAgainstQsort();
    testSizeSweep();
    testStability();
    testTinyStabilityExample();
    testPivotModes();
    testBenchTools();
    testCounters();
    printf("%d checks, %d failures\n", checks, failures);
    return failures != 0;
}
