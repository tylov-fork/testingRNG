#ifndef ROMUTRIO_H
#define ROMUTRIO_H

#include "splitmix64.h"

/**
*/
#define ROTL_(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

static uint64_t g_romutrio[3] = {
    0x26aa069ea2fb1a4dULL, 0x70c72c95cd592d04ULL,
    0x504f333d3aa0b359ULL,
};

static inline uint64_t romutrio(void) {
   uint64_t *s = g_romutrio, xp = s[0], yp = s[1], zp = s[2];
   s[0] = 15241094284759029579u * zp;
   s[1] = yp - xp; s[1] = ROTL_(s[1], 12);
   s[2] = zp - yp; s[2] = ROTL_(s[2], 44);
   return xp;    
}

static inline void romutrio_seed(uint64_t seed) {
  g_romutrio[0] = splitmix64_stateless(seed);
  g_romutrio[1] = splitmix64_stateless(seed + 1);
  g_romutrio[2] = splitmix64_stateless(seed + 2);
}

#endif
