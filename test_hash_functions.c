// test_hash_functions.c — known answers and range checks
#include "hash_functions.h"

#include <assert.h>
#include <stdio.h>

static uint64_t state = 42;
static uint64_t xorshift64(void) {               // tiny PRNG for the test
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}

int main(void) {
    // Known answers
    assert(hash_poly31("abc") == 96354);                   // Java "abc".hashCode()
    assert(hash_djb2("abc") == 193485963u);
    assert(hash_fnv1a("") == 0xcbf29ce484222325u);          // offset basis
    assert(hash_fnv1a("a") == 0xaf63dc4c8601ec8cu);         // published test vector
    assert(hash_fmix64(0) == 0);
    assert(hash_sum("abc") == hash_sum("cab"));             // order-blind
    assert(hash_poly31("abc") != hash_poly31("cab"));       // order matters

    // Results always land in 0 .. m-1
    universal_hash u = universal_pick(xorshift64);
    assert(u.a >= 1 && u.a < 2147483647u && u.b < 2147483647u);
    for (uint64_t k = 0; k < 100000; k++) {
        assert(hash_division(k, 1021) < 1021);
        assert(hash_fibonacci(k, 10) < 1024);
        assert(hash_universal(u, k, 1000) < 1000);
    }
    puts("all hash function tests passed");
    return 0;
}
