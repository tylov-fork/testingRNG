#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "aesctr.h"
#include "xorshift32.h"
#include "pcg32.h"
#include "xorshift128plus.h"
#include "xorshift1024star.h"
#include "xorshift1024plus.h"
#include "xoroshiro128plus.h"
#include "splitmix64.h"
#include "pcg64.h"
#include "lehmer64.h"
#include "mersennetwister.h"
#include "mitchellmoore.h"
#include "xorshift-k4.h"
#include "xorshift-k5.h"
#include "widynski.h"
#include "aesdragontamer.h"
#include "wyhash.h"
#include "wyrand.h"
#include "trivium32.h"
#include "trivium64.h"
#include "stc64.h"
#include "romutrio.h"


#ifndef __x86_64__
#warning "Expecting an x64 processor."
#endif


#define RDTSC_START(cycles)                                                    \
  do {                                                                         \
    register unsigned cyc_high, cyc_low;                                       \
    __asm volatile("cpuid\n\t"                                                 \
                   "rdtsc\n\t"                                                 \
                   "mov %%edx, %0\n\t"                                         \
                   "mov %%eax, %1\n\t"                                         \
                   : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx",    \
                     "%rdx");                                                  \
    (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                           \
  } while (0)

#define RDTSC_FINAL(cycles)                                                    \
  do {                                                                         \
    register unsigned cyc_high, cyc_low;                                       \
    __asm volatile("rdtscp\n\t"                                                \
                   "mov %%edx, %0\n\t"                                         \
                   "mov %%eax, %1\n\t"                                         \
                   "cpuid\n\t"                                                 \
                   : "=r"(cyc_high), "=r"(cyc_low)::"%rax", "%rbx", "%rcx",    \
                     "%rdx");                                                  \
    (cycles) = ((uint64_t)cyc_high << 32) | cyc_low;                           \
  } while (0)

/*
 * Prints the best number of operations per cycle where
 * test is the function call, answer is the expected answer generated by
 * test, repeat is the number of times we should repeat and size is the
 * number of operations represented by test.
 */
#define BEST_TIME(test, prec, repeat, size, nbytes)                            \
  do {                                                                         \
    printf("%-40s: ", #test);                                                  \
    fflush(NULL);                                                              \
    test##_seed(1234567890123ull);                                             \
    uint64_t cycles_start, cycles_final, cycles_diff;                          \
    uint64_t min_diff = (uint64_t)-1;                                          \
    for (int i = 0; i < repeat; i++) {                                         \
      RDTSC_START(cycles_start);                                               \
      for (size_t j = 0; j != size; ++j) prec[j] = test();                     \
      RDTSC_FINAL(cycles_final);                                               \
      cycles_diff = (cycles_final - cycles_start);                             \
      if (cycles_diff < min_diff)                                              \
        min_diff = cycles_diff;                                                \
    }                                                                          \
    uint64_t S = size * (nbytes);                                              \
    float cycle_per_op = (min_diff) / (double)S;                               \
    printf(" %.2f cycles per byte\n", cycle_per_op);                           \
    fflush(NULL);                                                              \
  } while (0)

void rand_seed(size_t seed) { srand((unsigned) seed); }

#define BT(test, nbytes) \
  BEST_TIME(test, prec, repeat, size, nbytes)

void demo(size_t size, int repeat) {
  printf("Generating %" PRIuMAX " random numbers, %d repititions.\n", size, repeat);
  printf("Time reported in number of cycles per byte generated by rng.\n");
  printf("We store values to an array of size = %" PRIuMAX " 64-bit numbers.\n\n", size);
  uint64_t *prec = malloc(size * sizeof *prec);

  BT(trivium32,4); BT(xorshift_k4,4); BT(xorshift_k5,4); BT(mersennetwister,4);
  BT(mitchellmoore,4); BT(widynski,4); BT(xorshift32,4); BT(pcg32,4); BT(rand,4);
  puts("");
  BT(trivium64,8); BT(aesdragontamer,8); BT(aesctr,8); BT(lehmer64,8); BT(xorshift128plus,8);
  BT(xoroshiro128plus,8); BT(splitmix64,8); BT(pcg64,8); BT(xorshift1024star,8); BT(xorshift1024plus,8);
  BT(wyhash64,8); BT(wyrand,8);
  puts("");
  BT(sfc64,8); BT(stc64,8); BT(romutrio,8);

  uint64_t sum = 0;
  for (size_t i=0; i<size; ++i) sum += prec[i];
  printf("\nSum %" PRIxMAX "\n", sum);
  free(prec);
}

int main() {
  enum {N=1000, R=500};

  demo(N, R);
  demo(N, R);
  printf("We execute the benchmark twice. Make sure that you get "
         "comparable results.\n");
}
