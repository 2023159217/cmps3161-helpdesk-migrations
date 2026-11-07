// measure_hashes.c — same keys, same table size, different hash functions
#include "hash_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { N = 50000, M = 65536, M_PRIME = 65521, BITS = 16, REPS = 200 };

static uint64_t ikeys[N];
static char     skeys[N][16];
static uint32_t count[M];

static void report(const char *name, size_t m) {     // count[] already filled
    size_t used = 0, longest = 0;
    double cmp = 0;
    for (size_t b = 0; b < m; b++) {
        if (count[b] > 0) used++;
        if (count[b] > longest) longest = count[b];
        cmp += (double)count[b] * (double)(count[b] + 1) / 2.0;
    }
    printf("  %-26s m=%-6zu used=%6zu longest=%5zu avg_cmp=%9.2f\n",
           name, m, used, longest, cmp / N);
}

static uint64_t state = 2026;
static uint64_t xorshift64(void) {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

static void int_suite(const char *title) {
    printf("%s\n", title);
    memset(count, 0, sizeof count);
    for (size_t i = 0; i < N; i++) count[hash_division(ikeys[i], M)]++;
    report("k % 65536 (power of 2)", M);
    memset(count, 0, sizeof count);
    for (size_t i = 0; i < N; i++) count[hash_division(ikeys[i], M_PRIME)]++;
    report("k % 65521 (prime)", M_PRIME);
    memset(count, 0, sizeof count);
    for (size_t i = 0; i < N; i++) count[hash_fibonacci(ikeys[i], BITS)]++;
    report("Fibonacci, top 16 bits", M);
    memset(count, 0, sizeof count);
    for (size_t i = 0; i < N; i++) count[hash_fmix64(ikeys[i]) & (M - 1)]++;
    report("fmix64, low 16 bits", M);
    for (size_t i = 0; i < N; i++) {
        if (ikeys[i] >= 2147483647u) {               // outside the family
            printf("  %-26s n/a: keys exceed p = 2^31 - 1\n", "universal");
            return;
        }
    }
    universal_hash u = universal_pick(xorshift64);
    memset(count, 0, sizeof count);
    for (size_t i = 0; i < N; i++) count[hash_universal(u, ikeys[i], M)]++;
    report("universal", M);
}

typedef uint64_t (*str_hash)(const char *);
static uint64_t w_sum(const char *s)   { return hash_sum(s); }
static uint64_t w_poly(const char *s)  { return hash_poly31(s); }
static uint64_t w_djb2(const char *s)  { return hash_djb2(s); }
static uint64_t w_fnv(const char *s)   { return hash_fnv1a(s); }

static double now(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static void str_one(const char *name, str_hash f) {
    memset(count, 0, sizeof count);
    for (size_t i = 0; i < N; i++) count[f(skeys[i]) % M]++;
    report(name, M);
}

static void str_suite(const char *title) {
    printf("%s\n", title);
    str_one("sum of characters", w_sum);
    str_one("poly31 (Java hashCode)", w_poly);
    str_one("djb2", w_djb2);
    str_one("FNV-1a", w_fnv);
}

static void str_speed(void) {
    const char *names[] = { "sum", "poly31", "djb2", "FNV-1a" };
    str_hash fs[] = { w_sum, w_poly, w_djb2, w_fnv };
    printf("speed (8-char IDs, %d x %d hashes)\n", REPS, N);
    for (size_t j = 0; j < 4; j++) {
        volatile uint64_t sink = 0;
        double t0 = now();
        for (int r = 0; r < REPS; r++)
            for (size_t i = 0; i < N; i++) sink = sink ^ fs[j](skeys[i]);
        double ns = (now() - t0) * 1e9 / ((double)REPS * N);
        printf("  %-8s %.2f ns per key\n", names[j], ns);
    }
}

int main(void) {
    printf("n = %d keys, alpha = %.3f, ideal avg_cmp = 1 + alpha/2 = %.2f\n\n",
           N, (double)N / M, 1.0 + (double)N / M / 2.0);

    for (size_t i = 0; i < N; i++)                     // student IDs as numbers
        ikeys[i] = (2019 + i / 6250) * 10000 + i % 6250;
    int_suite("A. integer student IDs 2019xxxx..2026xxxx");

    for (size_t i = 0; i < N; i++)                     // 16-byte aligned addresses
        ikeys[i] = 0x7f3a20000000u + 16u * i;
    int_suite("\nB. addresses that are multiples of 16 (like malloc results)");

    for (size_t i = 0; i < N; i++)
        snprintf(skeys[i], sizeof skeys[i], "%04zu%04zu", 2019 + i / 6250, i % 6250);
    str_suite("\nC. student IDs as strings");

    for (size_t i = 0; i < N; i++)
        snprintf(skeys[i], sizeof skeys[i], "item%zu", i);
    str_suite("\nD. names item0 .. item49999");

    for (size_t i = 0; i < N; i++)
        snprintf(skeys[i], sizeof skeys[i], "%04zu%04zu", 2019 + i / 6250, i % 6250);
    printf("\n");
    str_speed();
    return 0;
}
