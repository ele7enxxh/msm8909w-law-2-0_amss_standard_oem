#ifndef _QRLBN_ARITH_TYPES_H
#define _QRLBN_ARITH_TYPES_H
/*

  Copyright © 2013 Qualcomm Technologies Inc. 
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

//#define restrict 
//#define assert(ignore) ((void) 0)

/* 
 * Define EXPLICIT_INT_TYPES if the environment does not
 * have sys/types.h and, if necessary, stdint.h
 */
/*
#if !defined(size_t) 
typedef unsigned int size_t ;
#endif

#if !defined(int8_t) 
typedef signed char int8_t;
#endif

#if !defined(uint8_t) 
typedef unsigned char uint8_t;
#endif

#if !defined(int16_t) 
typedef short int16_t;
#endif

#if !defined(uint16_t) 
typedef unsigned short uint16_t;
#endif

#if !defined(uint32_t) 
typedef unsigned int uint32_t;
#endif

#if !defined(int32_t) 
typedef int int32_t;
#endif

#if !defined(int64_t) 
typedef long long int64_t;
#endif

#if !defined(uint64_t) 
typedef unsigned long long uint64_t;
#endif
*/
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


// The expectation is that some applications will using malloced memory, 
// and will want pointers.
// Other applications (particularly ECC code) will be using a structure 
// like this:
//
// typedef struct {
//     modulus_data_t md;
//     uint32_t modulus[SOME_SIZE];
//     uint32_t montgomery_R[SOME_SIZE];
//     uint32_t montgomery_R2[SOME_SIZE];
// } some_type_t;
//
// There can be other fields, but those 4 fields must be present and 
// have those names.
// 
 
// values for the flags field of modulus_data_t.

#define QRLBN_MIN_MODULUS_BITS 41  // Force num_words >= 3  

// type used in modulus_data_t.

typedef struct {
   unsigned flags;
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
