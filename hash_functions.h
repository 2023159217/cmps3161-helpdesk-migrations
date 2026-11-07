// hash_functions.h — hash functions every CMPS 2131 student should know (C23)
#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>

// ---- integer keys -> bucket index ------------------------------------
size_t   hash_division(uint64_t k, size_t m);           // k % m
size_t   hash_fibonacci(uint64_t k, unsigned bits);      // m = 2^bits
uint64_t hash_fmix64(uint64_t k);                        // MurmurHash3 finalizer

typedef struct { uint64_t a, b; } universal_hash;        // one member of the family
universal_hash universal_pick(uint64_t (*rand64)(void));
size_t   hash_universal(universal_hash h, uint64_t k, size_t m);

// ---- string keys -> 32/64-bit hash code ---------------------------------
uint32_t hash_sum(const char *s);                        // bad on purpose
uint32_t hash_poly31(const char *s);                     // Java String.hashCode
uint32_t hash_djb2(const char *s);                       // Bernstein
uint64_t hash_fnv1a(const char *s);                      // FNV-1a, 64-bit

#endif
