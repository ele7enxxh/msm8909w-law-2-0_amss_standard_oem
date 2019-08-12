#ifndef _QRLBN_ECC_INTERNAL_H
#define _QRLBN_ECC_INTERNAL_H

/*

  Copyright © 2014 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  
*/

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

/*
 * NOT FOR USE OUTSIDE OF MODULE
 *
 * Very simple modular exponentiation.  Base and result are Montgomery
 * encoded. Just one bit at a time square and multiply.
 * work must point to a bignum that can be used for temporary storage.
 * Only use this when the exponent is known.
 */
int qrlbn_modexp_for_ecc(qrlbn_ecc_bigval_t *z, 
			 const qrlbn_ecc_bigval_t *base,
			 const qrlbn_ecc_bigval_t *exponent,
			 size_t exponent_bits,  // >= actual values
			 const qrlbn_modulus_data_t *mdp);


/*
 * Sets tgt to one modular square root of a.  Returns 0 on success, -2
 * if a is not a quadratic residue, and -1 on all other errors.
 * 
 * Perhaps this should be at the arithmetic layer.  But it needs a
 * number of things that for historical reasons are only available at
 * the ECC layer.  In addition, being at the ECC layer makes declaring
 * local bignums easy.  At the arith layer they need to be provided
 * externally and passed in, since at the arith layer the size is not
 * known.
 *
 * If a is a quadratice residue (i.e. square roots exist), sets tgt
 * to one of the two possibled square roots.  
 * Returns 0 on success, -2 if a is not a quadratic residue, and -1
 * on all other errors.
 */

int qrlbn_ecc_square_root(qrlbn_ecc_bigval_t * restrict tgt, 
			  const qrlbn_ecc_bigval_t *a,
			  const qrlbn_ecc_domain_t *dp);


/* Generates a private key, k, which is a big integer, and a public
 * key, P1, which is a point.  k will satisfy 1 <= k < bp_order, where
 * bp_order is the order of the curve's base point.  Returns 0 on
 * success, -1 on failure.  Causes of failure include RNG failure and
 * invalid data.
 */
int qrlbn_EC_keypair_generate_internal(qrlbn_ecc_bigval_t * restrict k, 
				       qrlbn_ecc_point_t * restrict P, 
				       const qrlbn_ecc_domain_t *dp);


#ifdef	__cplusplus
}
#endif


#endif
