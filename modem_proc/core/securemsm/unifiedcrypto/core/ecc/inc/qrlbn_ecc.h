#ifndef _QRLBN_ECC_H
#define _QRLBN_ECC_H

/*

  Copyright © 2013 Qualcomm Technologies Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

*/

#include "qrlbn_ecc_types.h"

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
int qrlbn_EC_keypair_generate(qrlbn_ecc_bigval_t * k, 
			      qrlbn_ecc_affine_point_t * P,
			      const qrlbn_ecc_domain_t *dp);


/* Generates a secret value, k, and a point, P1, to send to the other
 * party.  k will satisfy 1 <= k < bp_order, were bp_order is the
 * order of the curve's base point.  Returns 0 on success, -1 on failure.
 * Causes of failure incude RNG failure, and invalid data.
 */
int qrlbn_ECDH_generate(qrlbn_ecc_bigval_t * k, 
			qrlbn_ecc_affine_point_t * P, 
			const qrlbn_ecc_domain_t *dp);

/* 
 * ECDH_derive takes the point sent by the other party, and verifies
 * that it is a valid point.  If 1 <= k < bp_order and Q is a valid
 * point, not the point at infinity, it stores the X coordinate of k*Q
 * in *tgt and returns 0.  Otherwise it returns -1 and *tgt is set to
 * zero.  
 */ 
int qrlbn_ECDH_derive(qrlbn_ecc_bigval_t * tgt, 
		      const qrlbn_ecc_bigval_t *k, 
		      const qrlbn_ecc_affine_point_t *Q, 
		      const qrlbn_ecc_domain_t *dp);

/* 
 * Like qrlbn_ECDH_derive, except that the compuation uses the
 * cofactor, as descibed in ASNI X9.63 Section 5.4.2.  This is
 * equivalent to qrlbn_ECDH_derive on curves where the cofactor is 1.
 */ 
int qrlbn_cofactor_ECDH_derive(qrlbn_ecc_bigval_t * tgt, 
			       const qrlbn_ecc_bigval_t *k, 
			       const qrlbn_ecc_affine_point_t *Q, 
			       const qrlbn_ecc_domain_t *dp);

/* 
 * ECDSA_sign takes a bigval_t that is a numerical representation of a
 * message digest (hash) of the message to be signed, and the private
 * key; it computes a signature, which is stored in *sig. It returns 0
 * on success and -1 on failure.  Causes of failure include RNG
 * failure, and invalid data.
 */ 
int qrlbn_ECDSA_sign(qrlbn_ECDSA_sig_t * sig, 
	       const qrlbn_ecc_bigval_t *msgdgst, 
	       const qrlbn_ecc_bigval_t *privkey,
	       const qrlbn_ecc_domain_t *dp);


/* 
 * ECDSA_verify takes a bigval_t that is a numerical representation of
 * a message digest (hash) of a message, the public key, and the
 * signature to be verified; it returns B_TRUE if the signature is
 * valid, and B_FALSE otherwise.  B_FALSE can also be returned when
 * any of the data, including the domain, are corrupted.
 */ 
boolean_t qrlbn_ECDSA_verify(const qrlbn_ecc_bigval_t *msgdgst,  
		             const qrlbn_ecc_affine_point_t *pubkey, 
		             const qrlbn_ECDSA_sig_t *sig, 
		             const qrlbn_ecc_domain_t *dp);

/* 
 * Converts qrlbn_bigval_t src to a network order (big-endian) binary
 * byte array representation. Negative values are sign extended.  For
 * negative values -1 will be returned.  If tgtlen is too small to
 * hold the value, the result is truncated and -1 is returned.  On
 * success, 0 is returned.  The tag field indicates type of value
 * being converted.  *dp should be completely filled in.  
 */
int qrlbn_bigval_to_binary(uint8_t * tgt, 
		     const qrlbn_ecc_bigval_t *src, size_t tgtlen, 
			   const qrlbn_ecc_domain_t *dp, 
			   qrlbn_field_tag_t tag); 


/* 
 * Converts a network-order (big-endian) binary byte array at src of
 * length srclen to a bigval_t at dst.  The value is alway positive.
 * If the value can be represented in allowed space, 0 is returned,
 * otherwise -1 is returned.  The tag field indicates type of value
 * being converted.
 *
 * Normally the domain structures should be completely filled in.
 * However there are cases when you are actually processing the value
 * to fill it in that this is imposssible. So here are the precise
 * rules.
 * 
 * tag                 dependency in domain structure
 * ================    ========================================
 * qrlbn_tag_m         domain.modulus.md.num_words
 * qrlbn_tag_X         domain.modulus.md.num_words
 * qrlbn_tag_Y         domain.modulus.md.num_words
 * qrlbn_tag_a         domain.modulus.md.num_words
 * qrlbn_tag_b         domain.modulus.md.num_words
 * qrlbn_tag_hash      Domain must be fully initialized
 * qrnbn_tag_n         domain.base_point_order.md.num_words
 * qrlbn_tag_privkey   domain.base_point_order.md.num_words
 * qrlbn_tag_r         domain.base_point_order.md.num_words
 * qrlbn_tag_s         domain.base_point_order.md.num_words
 *
 * The tag qrlnb_tag_hash is special.  It implements the arcane rules
 * of ASNI X9.62.  Let b be the number of bits necessary to represent
 * the modulus (from dp->md.modulus.modulus_bits).  If the size of the
 * hash is less than or equal to b, the hash is interpreted directly
 * as a number.  Otherwise the left most b bits of the hash are
 * converted to a number. The hash value is interpreted as big-endian
 * by byte.
 */
int qrlbn_binary_to_bigval(qrlbn_ecc_bigval_t *dst, const void* src, 
			   size_t srclen,
			   const qrlbn_ecc_domain_t *dp, 
			   qrlbn_field_tag_t tag);

/*
 * Expects modulus and affine_base_point to be filled in.
 */
int qrlbn_ecc_domain_init(qrlbn_ecc_domain_t *dp, 
			  const qrlbn_ecc_bigval_t *a, 
			  const qrlbn_ecc_bigval_t *b);


int qrlbn_ecc_domain_init_hex(qrlbn_ecc_domain_t * dp, 
			      char *modulus, 
			      char *a, char* b, char *x, char *y, 
			      char *n, 
			      uint64_t cofactor);

#ifdef	__cplusplus
}
#endif


#endif
