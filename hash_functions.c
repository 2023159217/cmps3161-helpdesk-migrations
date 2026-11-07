// hash_functions.c — hash functions every CMPS 2131 student should know (C23)
#include "hash_functions.h"

// ---------------------------------------------------------------- integers

size_t hash_division(uint64_t k, size_t m) {
    return (size_t)(k % m);              // keep the remainder
}

// Knuth's multiplicative method with A = 2^64 / golden ratio.
// The top `bits` bits of the product are the most mixed.
size_t hash_fibonacci(uint64_t k, unsigned bits) {
    const uint64_t A = 11400714819323198485u;  // 2^64 / phi
    return (size_t)((k * A) >> (64 - bits));    // top bits
}

// MurmurHash3's 64-bit finalizer: every input bit affects every
// output bit. Reduce afterwards with % m or a mask.
uint64_t hash_fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdu;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53u;
    k ^= k >> 33;
    return k;
}

// Carter-Wegman universal family:
//   h(k) = ((a*k + b) mod p) mod m, p = 2^31 - 1 (prime).
// Keys must be < p, so a*k stays below 2^62: no overflow.
enum : uint64_t { UNIVERSAL_P = 2147483647u };

universal_hash universal_pick(uint64_t (*rand64)(void)) {
    universal_hash h;
    h.a = 1 + rand64() % (UNIVERSAL_P - 1);   // 1 .. p-1
    h.b = rand64() % UNIVERSAL_P;             // 0 .. p-1
    return h;
}

size_t hash_universal(universal_hash h, uint64_t k, size_t m) {
    return (size_t)(((h.a * k + h.b) % UNIVERSAL_P) % m);
}

// ----------------------------------------------------------------- strings

uint32_t hash_sum(const char *s) {          // order-blind: avoid
    uint32_t h = 0;
    for (; *s != '\0'; s++) h += (unsigned char)*s;
    return h;
}

// h = s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]  (mod 2^32)
uint32_t hash_poly31(const char *s) {
    uint32_t h = 0;
    for (; *s != '\0'; s++) h = 31 * h + (unsigned char)*s;
    return h;
}

uint32_t hash_djb2(const char *s) {
    uint32_t h = 5381;
    for (; *s != '\0'; s++) h = 33 * h + (unsigned char)*s;
    return h;
}

uint64_t hash_fnv1a(const char *s) {
    uint64_t h = 14695981039346656037u;     // offset basis
    for (; *s != '\0'; s++) {
        h ^= (unsigned char)*s;             // fold in one byte
        h *= 1099511628211u;                // FNV prime
    }
    return h;
}
