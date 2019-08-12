#ifndef _QRLBN_ARITH_TYPES_H
#define _QRLBN_ARITH_TYPES_H
/*

  Copyright © 2014 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#include <stdint.h>
#include <stddef.h> // for ptrdiff_t and size_t
#include "uc_comdef.h"
#include "uc_env.h"

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

/*
 * C99 defines _Bool, but you have to include <stdbool.h> 
 * and then you get true and false, all lower case.
 * Since bools are in this top level .h file, it could
 * cause symbol conflicts with other definitions of true 
 * and false in programs that include this file.  So
 * we make our own.
 */
//typedef enum {B_FALSE, B_TRUE} boolean_t;

/*
 * Bignums are signed large integers.  They are represented as arrays
 * of uint32_t values in little endian order.  (The uint32_t values themselves 
 * are in native endian order.)  The most significant (i.e. last) value 
 * is always cast to int32_t.  
 *
 * Each bignum is interpreted in the context of a modulus_data_t type
 * that indicates the number of words.  In any give context all the
 * bignum variables effectively have the same size.
 */

// #define QRLBN_NON_SENSITIVE 1  // now in qrlbn_general.h 
#define QRLBN_MIN_MODULUS_BITS 41  // Force num_words >= 3  

// values for the flags field of modulus_data_t.

#define QRLBN_MD_m_inv_set    4
#define QRLBN_MD_m_recip_set  8   // actually both m_recip and quo_num_shift

typedef struct {
   unsigned flags;
   size_t allocated_words;
   size_t num_words;  // used words in bignums
   size_t modulus_bits;
   uint32_t *modulus;
   uint32_t *montgomery_R;
   uint32_t *montgomery_R2;
   uint32_t m_inv32; // used in computing qi for montgomery multiplication
   // int hi;  // The msb of the modulus lies in the lower 8 bits of m[hi]
            // or the upper 24 bits of m[hi - 1].
            // i.e. hi = (modulusbits + 24) / 32
   int quo_num_shift;  // Number of bits to shift {a[hi], a[hi-1]} in reduce()
                       // so that the binary point is just left of bit 7.
                       // i.e. quo_num_shift = modulusbits + 24 - 32 * hi
   uint64_t m_recip;  // Assuming binary point of modulus just left of 
   // modulus MSB, i.e. the modulus is a fraction >= 1/2 and < 1. 
   // The bit of weight 1 on the reciprocal is bit 60 of m_recip.
   // m_recip is >= 1 and <= 2.  (You would think > 1 because the modulus
   // < 1, but that modulus is very long so the rounded value of the recip 
   // can equal 1.  We really only need the high 28 bits or so, but some day
   // we might want to do a full Barrett reduce.
} qrlbn_modulus_data_t;

#ifdef	__cplusplus
}
#endif


#endif
