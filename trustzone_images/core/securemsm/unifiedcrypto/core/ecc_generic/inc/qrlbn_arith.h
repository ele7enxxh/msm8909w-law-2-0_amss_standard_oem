#ifndef _QRLBN_ARITH_H
#define _QRLBN_ARITH_H

/*

  Copyright © 2014 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#if defined(__CC_ARM) || defined(_MSC_VER)
  #define restrict 
#endif

#include "qrlbn_arith_types.h"
#include "qrlbn_arith_tags.h"

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

// Just a value for sanity checking.  Actual size definition
// is at a higher level.  Feel free to change this if it ever gets too small.
#define QRLBN_MAX_MODULUS_WORDS  (512+1)  // big enough for 16K bit RSA!!


// mdp validaiton macro.  Returns nonzero if mdp is null or 
// the qrlbn_modulus_data_t object has some structural problem
#define QRLBN_MD_CHECK_FAIL(mdp) \
   ((mdp) == NULL ||  					\
    (mdp)->modulus == NULL ||				\
    (mdp)->montgomery_R == NULL ||			\
    (mdp)->montgomery_R2 == NULL ||			\
    (mdp)->allocated_words > QRLBN_MAX_MODULUS_WORDS)

// no attempt to catch overflow here
#define QRLBN_MODBITS_TO_WORDS(nbits) (((nbits) + 24 + 31) / 32)

#define qrlbn_is_negative(a, mdp) \
  ((int32_t)(a)[(mdp)->num_words - 1] < 0)


boolean_t qrlbn_is_zero(const uint32_t *a, const qrlbn_modulus_data_t *mdp);



// qrlbn_add and qrlbn_sub do ordinary (not modular) add and subtract.
// z, a, and b can alias exaclty.
// Returns 0 in success or -1 on overfow.
// But don't let |sum| be greater than 2^23 or so times the modulus, 
// or you won't be able to reduce it.
int qrlbn_add(uint32_t * restrict z, const uint32_t *a, 
	      const uint32_t *b,  
	      const qrlbn_modulus_data_t *mdp);
int qrlbn_sub(uint32_t *z, const uint32_t *a, 
	      const uint32_t *b,  
	      const qrlbn_modulus_data_t *mdp);


// Computes z = a * b + c, where b and c are single words, signed.
// No reduction is done.  
// Returns 0 in success or -1 on overfow.
// But don't let |z| become greater than 2^23 or so times the modulus, 
// or so or you won't be able to reduce it.
// z and a can alias.
int qrlbn_multiply_add_small(uint32_t *z, 
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
int qrlbn_reduce(uint32_t *z, const uint32_t *a, 
		 const qrlbn_modulus_data_t *mdp, 
		 int flags);


// Montgomery multiplication.  z gets a * b mod m in montgomery space  
// In the raw space, z gets a * b / R mod m.
// a and b must be non-negative. 
// Returns 0 on success, -1 on error or overflow.
// z cannot alias a or b. a and b can alias.
// Using the raw values, on success z <= min(a, b) + m.

int qrlbn_mont_mul(uint32_t * restrict z, 
		   const uint32_t *a, const uint32_t *b,  
		   const qrlbn_modulus_data_t *mdp);


// This multiplies a by R^(-1), which is just 1 in the raw space.   This 
// transforms a value from Montgomery encoding to normal encoding.  The
// value of 'a' must be non-negative.
// Returns 0 on success, -1 on error.  

int qrlbn_mont_mul_one(uint32_t * restrict z, 
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
 * Right shifts a by k bits, storing the result in z.  a and z can
 * alias.  Sign extends if a is negative.  This is NOT modular shift
 * right.  The rightmost k bits are discarded.  Always succeeds,
 * returning 0.
 */
int qrlbn_shift_right(uint32_t *z, const uint32_t *a, unsigned k, 
		      const qrlbn_modulus_data_t *mdp);



/*
 * Sets z to 2^k, modlulo the modulus.  Leaks timing info.  
 * So when leakage resistance is important, k must not be sensitive.
 * Returns 0 on success, -1 on error.
 */
int qrlbn_modular_2_to_k(uint32_t * restrict z, unsigned k, const qrlbn_modulus_data_t *mdp);

// z and a must not alias.  a is an array of N_a uint32_t values.
// z is set to a mod mdp->modulus.
// If the QRLBN_ARG_UNSIGNED flag is supplied,
// a is considered an unsigned value, even if the MSB is set.
int qrlbn_full_reduce(uint32_t *restrict z, const uint32_t *a, size_t N_a, 
		      const qrlbn_modulus_data_t *mdp, int flags);

#define QRLBN_ARG_UNSIGNED 1


/*
  Initializing a qrlbn_modulus_data_t structure proceeds in several
  steps.  First, call qrlbn_modulus_data_pre_init.  This fills in
  pointers and sizes.  Then fill in the modulus using any of the data
  conversion functions.  The value must be zero padded out to
  num_allocated_words.  Then call qrlbn_modulus_data_post_init.  This
  computes a bunch of internal helper values.
  */

// Fill in pointers and lengths
int qrlbn_modulus_data_constructor(qrlbn_modulus_data_t *mdp, 
				   size_t bignum_allocated_words, 
				   uint32_t *modulus_storage, 
				   uint32_t *montR_storage, 
				   uint32_t *montR2_storage); 


/* 
 * qrlbn_modulus_data_pre_init must have been called, and the modulus
 * must have been filled in.  The modulus must be zero-padded out
 * though bignum_allocated words supplied in
 * qrlbn_modulus_data_pre_init.  This function fills various helper
 * values. If flags contains QRLBN_NO_MONT The montgomery-related
 * values are not filled in.  This flag MUST be supplied
 * if the modulus is even, which happens in some Chinese Remainder
 * Theorem computations.
 */

int qrlbn_modulus_data_post_init(qrlbn_modulus_data_t *mdp, int flags);


/*
 * Provides miscellaneous functions---kind of like ioctl for drivers.
 */
int qrlbn_modulus_data_ctl(qrlbn_modulus_data_t *mdp, int cmc, void *arg);


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
			 uint32_t * restrict *workbuffer_array,
			 const qrlbn_modulus_data_t *mdp);

/*
 * Like memcmp.  Not constant time.  Used only for checking that
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
int qrlbn_binary_to_bn(uint32_t * restrict dst, 
			   const uint8_t* src, size_t srclen,
			   size_t num_words); 

/*
 * converts a string of ascii characters encoding a value in hex to
 * a big number.  N is the allocated length of the target.
 * On success returns number of words filled.  On failure returns -1.
 */
int qrlbn_hex_string_to_bn(uint32_t * restrict bn, const char *hexstr, size_t N);


/*
 * Converts a string of ascii characters encoding a value in decimal
 * to a big number. N is the allocated length of the target in words.
 * However, the most significant bit is the sign bit. On success
 * returns 1.  On failure returns -1.
 */
int qrlbn_decimal_string_to_bn(uint32_t * restrict bn, const char *decstr, 
			       size_t N);


/*
 * returns 0 if the full value (out to allocated_words) is less than
 * the modulus.  otherwise returns -1.  Only for non-negative values.
 * Does not run in constant time.  Use only on public values.
 */
int qrlbn_vet(uint32_t *val, qrlbn_modulus_data_t *mdp);





#ifdef	__cplusplus
}
#endif


#endif
