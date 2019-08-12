#ifndef _QRLBN_ARITH_H
#define _QRLBN_ARITH_H

/*

  Copyright © 2013 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#include "qrlbn_arith_types.h"


#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

#define qrlbn_is_negative(a, mdp) \
  ((int32_t)(a)[(mdp)->num_words - 1] < 0)


boolean_t qrlbn_is_zero(const uint32_t *a, const qrlbn_modulus_data_t *mdp);



// qrlbn_add and qrlbn_sub do ordinary (not modular) add and subtract.
// z, a, and b can alias exaclty.
// Returns 0 in success or -1 on overfow.
// But don't let |sum| be greater than 2^23 or so times the modulus, 
// or you won't be able to reduce it.
int qrlbn_add(uint32_t *  z, const uint32_t *a, 
	      const uint32_t *b,  
	      const qrlbn_modulus_data_t *mdp);
int qrlbn_sub(uint32_t *z, const uint32_t *a, 
	      const uint32_t *b,  
	      const qrlbn_modulus_data_t *mdp);


// Computes z = a * b, where b is is in a single word, signed.
// No reduction is done.  
// Returns 0 in success or -1 on overfow.
// But don't let |z| become greater than 2^23 or so times the modulus, 
// or so or you won't be able to reduce it.
// z and a can alias.
int qrlbn_multiply_small_b(uint32_t *z, 
			   const uint32_t *a, int32_t b, int32_t c,
			   const qrlbn_modulus_data_t *mdp);

/*
 * a += k * mdp->modulus;  |k| is limited to around 2^29
 * Returns 0 on success, -1 on overflow.
 */
int qrlbn_modular_adjust(uint32_t *a, const uint32_t *b, int k, 
			 const qrlbn_modulus_data_t *mdp);


// These macros define flags for the reduce() function.
#define DO_CORRECTIVE_ADD 1
#define TARGET_ZERO 2
#define TARGET_POSITIVE 4
#define EXACT (TARGET_ZERO | DO_CORRECTIVE_ADD)

/*
 * Reduces z (in place) modulo mdp->modulus.  The flags argument
 * controls the reduction.
 */
int qrlbn_reduce(uint32_t *z, const uint32_t *a, const qrlbn_modulus_data_t *mdp, 
		 int flags);


// Montgomery multiplication.  z gets a * b mod m in montgomery space  
// In the raw space, z gets a * b / R mod m.
// a and b must be non-negative. 
// Returns 0 on success, -1 on error or overflow.
// z cannot alias a or b. a and b can alias.
// Using the raw values, on success z <= min(a, b) + m.

int qrlbn_mont_mul(uint32_t *  z, 
		   const uint32_t *a, const uint32_t *b,  
		   const qrlbn_modulus_data_t *mdp);


// This multiplies a by R^(-1), which is just 1 in the raw space.   This 
// transforms a value from Montgomery encoding to normal encoding.  The
// value of 'a' must be non-negative.
// Returns 0 on success, -1 on error.  

int qrlbn_mont_mul_one(uint32_t *  z, 
		       const uint32_t *a,
		       const qrlbn_modulus_data_t *mdp);

// z gets (a << k) + b mod mdp->modulus.  flags affect result as with reduce. 
// a and z can alias.  When leakage resisisance is important, k must 
// not be sensitive.  |b| must not be larger than about 2^50.
// Returns 0 on success, -1 on error. 
int qrlbn_modular_shift_left(uint32_t *z, const uint32_t *a, unsigned k,
			     int64_t b,
			     const qrlbn_modulus_data_t *mdp, int flags);

/*
 * Sets z to 2^k, modlulo the modulus.  Leaks timing info.  
 * So when leakage resistance is important, k must not be sensitive.
 * Returns 0 on success, -1 on error.
 */
int qrlbn_modular_2_to_k(uint32_t *  z, unsigned k, const qrlbn_modulus_data_t *mdp);

// z and a must not alias.  a is an array of N_a uint32_t values.
// z is set to a mod mdp->modulus.
// If the QRLBN_ARG_UNSIGNED flag is supplied,
// a is considered an unsigned value, even if the MSB is set.
int qrlbn_full_reduce(uint32_t * z, const uint32_t *a, size_t N_a, 
		      const qrlbn_modulus_data_t *mdp, int flags);

#define QRLBN_ARG_UNSIGNED 1

// accepts a partially filled in qrlbn_modulus_data_t and completes it.  
// num_alloc_words is number of words allocated for each bignum.
// (Actually, it is the maximum value for the num_words field, the
// actually allocated space must be at least this big.)
// The flags field must be set. The modulus montgomery_R, and 
// montgomery_R2 fields must be set to point to storage be set to 
// an offset to storage,
// The actual modulus bits must be
// set.  This function fills in the remaining
// fields.  
//
// If the flags arg contains QRLBN_MAKE_POINTER, offsets are converted to
// pointer.  (Mostly for easy of debugging.)
//
// Returns 0 on success, -1 on error.  Some causes of error include
// modulus too large for num_allocated_words, modulus not odd,
// modulus too small (smallest allowed value is 2^40 + 1).   
// 
// In ECC it is standard practice to use a specified curve, and
// hence it would be desirable to compute the initialzed value 
// at design time, and then put the values into an static initializer.
// This can work if the flags word does not call for any pointers.
int qrlbn_modulus_data_init(qrlbn_modulus_data_t *mdp, size_t num_allocated_words);


/*
 * Provides miscellaneous functions---kind of like ioctl for drivers.
 * Mostly for debugging.
 * If QRLBN_MAKE_POINTER is supplied, offsets are replaced by pointers. 
 */
int qrlbn_modulus_data_ctl(qrlbn_modulus_data_t *mdp, int cmc, void *arg);
#define QRLBN_MAKE_POINTERS 1


/*
 * This uses the extended binary GCD (Greatest Common Divisor)
 * algorithm.  The binary GCD algorithm is presented in [KnuthV2] as
 * Algorithm X.  The extension to do division is presented in Homework
 * Problem 15 and its solution in the back of the book.
 *
 * The implementation here follows the presentation in [HMV] Algorithm
 * 2.22 with the added condition of rv == 0 in the outer while loop to
 * prevent infinite looping when the denominator divides the modulus.
 *
 * Modulus must be odd.  The denominator must be positive.
 * and must not divide the modulus.  If num is a null pointer,
 * the numeratator is treated as 1, i.e. it does inversion.
 * tgt, num, and den can all alias.
 *
 * workbuf is an array of 4 pointers to buffers of type uint32_t.
 * Each buffer must be at least mdp->num_words long.  These must
 * not alias anying else.
 *
 * Returns -1 on reduce error or the denominator divides the modulus. 
 * Otherwise returns 0.  
 *
 * This is not constant time, so you must take precautions where
 * the argument is a senstive value.  One possible precaution is
 * to note that k^(-1) = b * (b*k)^(-1), where b is a random value
 * such that 1 <= b < order.  Do not simply put b in the numerator.
 * There is no guarantee that this does not leak, and if b leaks
 * we lose our security.   
 */
int qrlbn_modular_divide(uint32_t *tgt, 
			 const uint32_t *num, const uint32_t *den,
			 uint32_t *  *workbuffer_array,
			 const qrlbn_modulus_data_t *mdp);

/*
 * Like memcmp.  Not cnstant time.  Used only for checking that
 * incoming data is valid, and that is public info.
 */

int qrlbn_compare(const uint32_t *a, const uint32_t *b, 
		  const qrlbn_modulus_data_t *mdp);


/* 
 * Converts uint32_t src to a network order (big-endian) binary byte
 * array representation. Only positive values are suppored.  If the
 * value cannot be represented, -1 is returned.
 */
int qrlbn_bn_to_binary(uint8_t *tgt, const uint32_t *src, size_t tgt_bytes, size_t src_words);

/* 
 * Converts a network-order (big-endian) binary byte array at src of
 * length srclen to a bignum at dst.  The value is always positive.
 * If the value can be represented in num_words words, 0 is
 * returned, otherwise -1 is returned.
 *
 * This deviates from the usual format in that it accepts num_words rather
 * than a qrlbn_modulus_data_t pointer.
 */
int qrlbn_binary_to_bn(uint32_t *  dst, 
			   const uint8_t* src, size_t srclen,
			   size_t num_words); 

/*
 * converts a string of ascii characters encoding a value in hex to
 * a big number.  N is the allocated length of the target.
 * On success returns number of words filled.  On failure returns -1.
 */
int qrlbn_string_to_bn(uint32_t *  bn, const char *hexstr, size_t N);


#ifdef	__cplusplus
}
#endif


#endif
