#ifndef _QRLBN_ECC_INTERNAL_H
#define _QRLBN_ECC_INTERNAL_H

/*

  Copyright © 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  QUALCOMM Confidential and Proprietary
  
*/

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif


/* Generates a private key, k, which is a big integer, and a public
 * key, P1, which is a point.  k will satisfy 1 <= k < bp_order, where
 * bp_order is the order of the curve's base point.  Returns 0 on
 * success, -1 on failure.  Causes of failure include RNG failure and
 * invalid data.
 */
int qrlbn_EC_keypair_generate_internal(qrlbn_ecc_bigval_t * k, 
				       qrlbn_ecc_point_t * P, 
				       const qrlbn_ecc_domain_t *dp);


#define QRLBN_NON_SENSITIVE 1

#ifdef	__cplusplus
}
#endif


#endif
