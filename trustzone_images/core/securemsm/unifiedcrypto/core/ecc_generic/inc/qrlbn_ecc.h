#ifndef _QRLBN_ECC_H
#define _QRLBN_ECC_H

/*

  Copyright © 2015 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#include "qrlbn_ecc_types.h"
#include "qrlbn_arith.h"

#define QRLBN_ECC_MODULUS_CHECK_FAIL(modulus_struct_p)			\
  ((modulus_struct_p) == NULL ||					\
   (modulus_struct_p)->md.modulus != (modulus_struct_p)->modulus_storage.data || \
   (modulus_struct_p)->md.montgomery_R != (modulus_struct_p)->montgomery_R_storage.data || \
   (modulus_struct_p)->md.montgomery_R2 != (modulus_struct_p)->montgomery_R2_storage.data || \
   (modulus_struct_p)->md.allocated_words != _ECC_BIGNUM_WORDS)


#define QRLBN_ECC_DOMAIN_CHECK_FAIL(dp)				\
  ((dp) == NULL || (dp)->version != QRLBN_DATA_STRUCTURE_VERSION ||	\
   QRLBN_ECC_MODULUS_CHECK_FAIL(&(dp)->modulus) ||			\
   QRLBN_ECC_MODULUS_CHECK_FAIL(&(dp)->base_point_order))

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

/* Generates a secret value, k, and a point, P1. k will satisfy 1 <= k
 * < bp_order, were bp_order is the order of the curve's base point.
 * Returns 0 on success, -1 on failure.  Causes of failure include RNG
 * failure, and invalid data.
 */
int qrlbn_EC_keypair_generate(qrlbn_ecc_bigval_t * restrict k, 
			      qrlbn_ecc_affine_point_t * restrict P,
			      const qrlbn_ecc_domain_t *dp);


/* Generates a secret value, k, and a point, P1, to send to the other
 * party.  k will satisfy 1 <= k < bp_order, were bp_order is the
 * order of the curve's base point.  Returns 0 on success, -1 on failure.
 * Causes of failure incude RNG failure, and invalid data.
 */
int qrlbn_ECDH_generate(qrlbn_ecc_bigval_t * restrict k, 
			qrlbn_ecc_affine_point_t * restrict P, 
			const qrlbn_ecc_domain_t *dp);

/* 
 * ECDH_derive takes the point sent by the other party, and verifies
 * that it is a valid point.  If 1 <= k < bp_order and Q is a valid
 * point, not the point at infinity, it stores the X coordinate of k*Q
 * in *tgt and returns 0.  Otherwise it returns -1 and *tgt is set to
 * zero.  By default, the Y coordiate is ignored, which enables
 * compressed form communication.  It is used if the QRLBN_POINT_UNCOMPRESSED flag
 * is supplied.  This flag is forced false unless the
 * SUPPORT_USE_Y_COORDINATE_qJ2bH7agxPvox4Ca compile time flag is
 * supplied.
 */ 
int qrlbn_ECDH_derive(qrlbn_ecc_bigval_t * restrict tgt, 
		      const qrlbn_ecc_bigval_t *k, 
		      const qrlbn_ecc_affine_point_t *Q, 
		      const qrlbn_ecc_domain_t *dp, int flags);

/* 
 * Like qrlbn_ECDH_derive, except that the compuation uses the
 * cofactor, as descibed in ASNI X9.63 Section 5.4.2.  This is
 * equivalent to qrlbn_ECDH_derive on curves where the cofactor is 1.
 */ 
int qrlbn_cofactor_ECDH_derive(qrlbn_ecc_bigval_t * restrict tgt, 
			       const qrlbn_ecc_bigval_t *k, 
			       const qrlbn_ecc_affine_point_t *Q, 
			       const qrlbn_ecc_domain_t *dp, int flags);

/* 
 * ECDSA_sign takes a bigval_t that is a numerical representation of a
 * message digest (hash) of the message to be signed, and the private
 * key; it computes a signature, which is stored in *sig. It returns 0
 * on success and -1 on failure.  Causes of failure include RNG
 * failure, and invalid data.
 */ 
int qrlbn_ECDSA_sign(qrlbn_ECDSA_sig_t * restrict sig, 
	       const qrlbn_ecc_bigval_t *msgdgst, 
		     const qrlbn_ecc_bigval_t *privkey, 
	       const qrlbn_ecc_domain_t *dp);


/* 
 * ECDSA_verify takes a bigval_t that is a numerical representation of
 * a message digest (hash) of a message, the public key, and the
 * signature to be verified; it returns B_TRUE if the signature is
 * valid, and B_FALSE otherwise.  B_FALSE can also be returned when
 * any of the data, including the domain, are corrupted.
 * If flags contains QRLBN_ECDSA_POINT_COMPRESSED, only the 
 * LSB of the Y coordinate is used.
 */ 
boolean_t qrlbn_ECDSA_verify(const qrlbn_ecc_bigval_t *msgdgst,  
		             const qrlbn_ecc_affine_point_t *pubkey, 
		             const qrlbn_ECDSA_sig_t *sig, 
			     const qrlbn_ecc_domain_t *dp,
			     int flags);

/* 
 * Converts qrlbn_bigval_t src to a network order (big-endian) binary
 * byte array representation. Negative values are sign extended.  For
 * negative values -1 will be returned.  If tgtlen is too small to
 * hold the value, the result is truncated and -1 is returned.  On
 * success, 0 is returned.  The tag field indicates type of value
 * being converted.  *dp should be completely filled in.  
 */
int qrlbn_ECC_bigval_to_binary(uint8_t * restrict tgt, 
		     const qrlbn_ecc_bigval_t *src, size_t tgtlen, 
			   const qrlbn_ecc_domain_t *dp, 
			   qrlbn_field_tag_t tag); 


/* 
 * Converts input to bigval.  Input can be network order binary, ascii
 * hex, or ascii decimal, as indicated by radix (256, 16, 10,
 * respectively).  When mode is 256 (binary), srclen indicates the
 * length.  Otherwise, srclen is ignored and the input must be null
 * terminated.  Returns 0 on success, -1 on error.  If the data cannot
 * be represented, an error is indicated.
 */
int qrlbn_ECC_convert_input_to_bigval(qrlbn_ecc_bigval_t *dst, const void* src, 
				      size_t srclen, 
				      int radix);



/* 
 * returns a pointer to the appropriate qrlbn_ecc_bigval_t field in
 * *dp, where the field is selected according to tag.  On error, NULL
 * is returned.
 */

qrlbn_ecc_bigval_t* 
qrlbn_ECC_domain_field_pointer(qrlbn_ecc_domain_t *dp, 
			       qrlbn_field_tag_t tag);


/*
 * Converts a hash value to a bigval_t.  The rules for this in ANSI
 * X9.62 are strange.  Let b be the number of bits necessary to
 * represent the base_point_order (n).  If the size of the hash is less
 * than or equal to b, the hash is interpreted directly as a number.
 * Otherwise the left most b bits of the hash are converted to a
 * number. The hash value is interpreted as big-endian by byte.
 *
 * hashlen is in bytes.
 */
int qrlbn_ecc_hash_to_bigval(qrlbn_ecc_bigval_t * restrict dst, 
			     const uint8_t *hashp, 
			     size_t hashlen, 
			     const qrlbn_ecc_domain_t *dp);





/*
 * Zeroses dp and fills in pointers and other constant fields.
 *
 */

int qrlbn_ecc_domain_setup_structure(qrlbn_ecc_domain_t *dp);

/*
 * Fills in computed values in dp.  Checks that supplied values are in
 * range.  Expects modulus, base point order, affine_base_point to be
 * filled in. For most common curves, cofactor = 1.
 */
int qrlbn_ecc_domain_post_init(qrlbn_ecc_domain_t *dp, uint64_t cofactor);


/* Complete set up of domian structure in one call */
int qrlbn_ecc_domain_init_str(qrlbn_ecc_domain_t * restrict dp, 
			      const char *modulus, 
			      const char *a, const char* b, const char *x, 
			      const char *y, 
			      const char *n, 
			      uint64_t cofactor, int base);

#ifdef	__cplusplus
}
#endif


#endif
