#ifndef STC64_H
#define STC64_H

#include "splitmix64.h"

/**
*/

static uint64_t g_stc64[5] = {
    0x26aa069ea2fb1a4dULL, 0x70c72c95cd592d04ULL,
    0x504f333d3aa0b359ULL, 0x9e3779b97f4a7c15ULL,
    0x6a09e667a754166bULL,
};

static inline uint64_t stc64(void) {
    enum {LR=24, RS=11, LS=3}; uint64_t* s = g_stc64; 
    const uint64_t result = (s[0] ^ (s[3] += s[4])) + s[1];
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = ((s[2] << LR) | (s[2] >> (64 - LR))) + result;
    return result;
}

static inline uint64_t sfc64(void) {
    enum {LR=24, RS=11, LS=3}; uint64_t* s = g_stc64; 
    const uint64_t result = s[0] + s[1] + s[3]++;
    s[0] = s[1] ^ (s[1] >> RS);
    s[1] = s[2] + (s[2] << LS);
    s[2] = ((s[2] << LR) | (s[2] >> (64 - LR))) + result;
    return result;
}

/* g_stc64[4] must be odd */
static inline void stc64_seed(uint64_t seed) {
    g_stc64[0] = seed+0x26aa069ea2fb1a4d; 
    g_stc64[1] = seed+0x70c72c95cd592d04;
    g_stc64[2] = seed+0x504f333d3aa0b359;
    g_stc64[3] = seed, 
    g_stc64[4] = seed<<1 | 1;
    for (int i = 0; i < 6; ++i) stc64();
}

static inline void sfc64_seed(uint64_t seed) { stc64_seed(seed); }

#endif
