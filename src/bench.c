#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
#include "bench.h"

const char *SHAPE_NAMES[SHAPE_COUNT] = {"random", "sorted", "reversed", "nearly", "dups"};

/* 시간(ms). 정렬 코드는 시계를 들고 있지 않고, 여기서 바깥에서 잰다 */
static double nowMs(void) {
#ifdef _WIN32
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart * 1000.0 / (double)f.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
#endif
}

/* rand()는 플랫폼마다 다르므로(Windows는 최대 32767) 직접 만든 난수를 쓴다.
   덕분에 어느 컴퓨터에서 돌려도 입력이, 곧 비교·이동 횟수가 똑같다 */
static uint32_t rngNext(uint32_t *s) {
    uint32_t x = *s;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return *s = x;
}

int recordCompare(const void *a, const void *b) {
    const Record *x = (const Record *)a, *y = (const Record *)b;
    return (x->key > y->key) - (x->key < y->key);   /* tag까지 보면 모두 안정해 보인다 */
}

Record *makeInput(InputShape shape, size_t n, uint32_t seed) {
    Record *a = (Record *)malloc((n ? n : 1) * sizeof *a);
    uint32_t s = seed ? seed : 1;
    if (a == NULL) return NULL;
    for (size_t i = 0; i < n; i++) { a[i].key = (int)i; a[i].tag = (int)i; }

    switch (shape) {
    case SHAPE_RANDOM:                                   /* Fisher-Yates */
        for (size_t i = n; i > 1; i--) {
            size_t j = rngNext(&s) % i;
            int t = a[i - 1].key; a[i - 1].key = a[j].key; a[j].key = t;
        }
        break;
    case SHAPE_REVERSED:
        for (size_t i = 0; i < n; i++) a[i].key = (int)(n - 1 - i);
        break;
    case SHAPE_NEARLY:
        for (size_t k = 0; k < n / 100; k++) {
            size_t i = rngNext(&s) % n, j = rngNext(&s) % n;
            int t = a[i].key; a[i].key = a[j].key; a[j].key = t;
        }
        break;
    case SHAPE_DUPS:
        for (size_t i = 0; i < n; i++) a[i].key = (int)(rngNext(&s) % 16);
        break;
    default:   /* SHAPE_SORTED */
        break;
    }
    return a;
}

int isSortedByKey(const Record *a, size_t n) {
    for (size_t i = 1; i < n; i++)
        if (a[i - 1].key > a[i].key) return 0;
    return 1;
}

int isStable(const Record *a, size_t n) {
    for (size_t i = 1; i < n; i++)
        if (a[i - 1].key == a[i].key && a[i - 1].tag > a[i].tag) return 0;
    return 1;
}

BenchResult benchRun(const SortAlgorithm *algo, const Record *input, size_t n, int reps) {
    BenchResult r;
    Record *work = (Record *)malloc(n * sizeof *work);
    double total = 0;
    memset(&r, 0, sizeof r);
    if (work == NULL) return r;

    for (int k = 0; k < reps; k++) {
        memcpy(work, input, n * sizeof *work);           /* 복사는 시간에서 뺀다 */
        memset(&r.stats, 0, sizeof r.stats);
        double t0 = nowMs();
        algo->sort(work, n, sizeof(Record), recordCompare, &r.stats);
        total += nowMs() - t0;
    }
    r.ms = total / reps;
    r.sorted = isSortedByKey(work, n);
    r.stable = isStable(work, n);
    free(work);
    return r;
}
