#ifndef _QRLBN_UTILS_H
#define _QRLBN_UTILS_H

/*

  Copyright © 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  
*/


#include <stdint.h>

int micromalloc(uint32_t *state, int size);
void microfree(uint32_t *state, size_t size, int mem_item);

#define MICROBAD  0x456789a
#define MICROMALLOC() micromalloc(&mmstate, NUM_TEMPS)
#define MICROFREE(m) microfree(&mmstate, NUM_TEMPS, m); m = MICROBAD
//#define MICROFREE(m) microfree(&mmstate, NUM_TEMPS, m)
/*
 * Copies a selected one of 4 arrays to tgt.  The parameter src_array
 * points to an array of 4 pointers to the base of arrays.  The
 * selector-th array is copied to tgt.  All 4 are read, but the final
 * value is just one.  This resists cache line attacks.  The selection
 * is done by a logical AND with either 0 or 0xffffffff.  The mask
 * might be detectable using simple power analysis.  So the arrays
 * are not read and ANDed 0, 1, 2, 3, but rather the starting point is
 * rotated by the rnd value.
 */
void qrlbn_multiplexed_copy_arrays(uint32_t *dest,
				   uint32_t *src[],
				   int selector,
				   int rnd,
				   size_t num_words);

int find_MSB(const uint32_t *a, size_t num_words);

/* 
 * Returns the number of trailing zeros in v, where the last k bits
 * are forced to zero (k <= 31).  N is the size in words. If v is
 * zero, returns 32 * N.  Does not run in constant time.  Behavior is
 * undefined if k > 31.
 */
int qrlbn_count_trailing_zeros(const uint32_t *v, int k, size_t N);


void qrlbn_zeroize(volatile void* tgt, size_t len);
void qrlbn_zeroize_words(volatile uint32_t* ptr, size_t n_words);

#endif
