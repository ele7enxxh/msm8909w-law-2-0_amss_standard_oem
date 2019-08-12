#ifndef _ECC_CORE_H
#define _ECC_CORE_H

#include "uc_comdef.h"
#include "uc_env.h"
/*

  Copyright 2012 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

/* 
 * Define EXPLICIT_INT_TYPES if the environment does not
 * have sys/types.h and, if necessary, stdint.h
 */

#if 0
#if !defined(size_t) 
typedef unsigned int size_t ;
#endif
#endif

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

//typedef enum {B_FALSE, B_TRUE} boolean_t;

/* 
 * Define either ECC_P256 or ECC_P192 to control which polynomial is used.
 * Of course, the binary must have been compiled with the same .h file
 * contents as are used by applications.
 *
 * Arguably, to avoid branching in version control systems, this
 * should be supplied by a -DP256 or -DP192 at compile time. It would be
 * good to include a run-time check to make sure the
 * library was compiled with the same modulus as the include file
 * specifies, but it seems require that the main "functions" be
 * macros, and I'm not sure I want to do that.
 */

#define ECC_P256

#ifdef ECC_P256
#define BIGLEN 9
#endif

#ifdef ECC_P192
#define BIGLEN 7
#endif

#if !defined ECC_P256 && !defined ECC_P192
#error
#endif

/* 
 * For P256 bigval_t types hold 288-bit 2's complement numbers (9
 * 32-bit words).  For P192 they hold 224-bit 2's complement numbers
 * (7 32-bit words).
 * 
 * The representation is little endian by word and native endian
 * within each word.
 */

typedef struct {
      uint32 data[BIGLEN];
} bigval_t;


typedef struct {
      bigval_t x;
      bigval_t y;
      boolean_t infinity;
} affine_point_t;

/* Generates a secret value, k, and a point, P1, to send to the other
 * party.  k will satisfy 1 <= k < orderP.  Returns 0 on success, -1
 * on failure (of the RNG).
 */
int ECDH_generate(affine_point_t *P1, bigval_t *k);

/* 
 * k is given, not an output.
 */
int ECDH_mult_basepoint(affine_point_t *P1, const bigval_t *k);

/* 
 * ECDH_derive takes the point sent by the other party, and verifies
 * that it is a valid point.  If 1 <= k < orderP and the point is
 * valid, it stores the result in *tgt and returns B_TRUE.  If the
 * point is invalid it returns B_FALSE.  The behavior with k out of
 * range is unspecified, but safe, in the sense that
 * it will not cause any corruption.
 */ 
boolean_t ECDH_derive(bigval_t *tgt, bigval_t const *k, 
		      affine_point_t const *Q);

typedef struct {
      bigval_t r;
      bigval_t s;
} ECDSA_sig_t;


/*
 * Converts a hash value to a bigval_t.  The rules for this in
 * ANSIX9.62 are strange.  Let b be the number of octets necessary to
 * represent the modulus.  If the size of the hash is less than or
 * equal to b, the hash is interpreted directly as a number.
 * Otherwise the left most b octets of the hash are converted to a
 * number. The hash must be big-endian by byte. There is no alignment
 * requirement on hashp.
 */
void
ECC_hash_to_bigval(bigval_t *tgt, void const *hashp, unsigned int hashlen);


/* 
 * ECDSA_sign takes a bigval_t that is a numerical representation of
 * a message digest (hash) of the message to be signed,
 * and the private key; it computes a signature, which is
 * stored in *sig. (See  It returns 0 on success and -1 on failure.  (The
 * only cause of failure is failure of the RNG.)
 */ 
int ECDSA_sign(bigval_t const *msgdgst, 
	       bigval_t const *privkey, 
	       ECDSA_sig_t *sig);


/* 
 * ECDSA_verify takes a bigval_t that is a numerical representation of
 * a message digest (hash) of a message, the public key, and the
 * signature to be verified; it returns B_TRUE if the signature is
 * valid, and B_FALSE otherwise.
 */ 
boolean_t ECDSA_verify(bigval_t const *msgdgst,  
		       affine_point_t const *pubkey, 
		       ECDSA_sig_t const *sig);
/* 
 * Converts bigval_t src to a network order (big-endian) binary byte
 * array representation. The if tgtlen is longer that the bigval_t,
 * the value is sign-extended.  If tgtlen is too small to hold the
 * value, high order bytes are silently dropped.
 */
void bigval_to_binary(bigval_t const *src, void *tgt, size_t tgtlen);

/* 
 * Converts a network-order (big-endian) binary byte array at
 * *src of length srclen to a bigval_t at *dst.  If srclen is larger
 * then the length of a bigval_t, the high order bytes are silently
 * dropped.
 */
void binary_to_bigval(const void* src, bigval_t *dst, size_t srclen); 


/*
 * The external function get_random_bytes is expected to be avaiable.
 * It must return 0 on success, and -1 on error.  Feel free to raname
 * this function, if necessary.  
 */
int get_random_bytes(void *buf, int len);

/**
 * @brief inject error for testing 
 *
 * @param buff         [in] Data to inject error
 * @param len          [in] Length of data
 *
 * @return 0 on success, -1 on error
 *
 * @see 
 *
 */

int toggle_random_bit(void *buf, size_t len);

boolean_t ECDH_derive_pt(affine_point_t *tgt, bigval_t const *k, 
			 affine_point_t const *Q);


boolean_t 
in_curveP(affine_point_t const *P);

#endif /* _ECC_CORE_H */
