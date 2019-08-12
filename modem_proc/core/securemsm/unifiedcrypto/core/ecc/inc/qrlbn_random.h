#ifndef _QRLBN_RANDOM_H
#define _QRLBN_RANDOM_H

/*

  Copyright © 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  QUALCOMM Confidential and Proprietary
  
*/


#include "qrlbn_arith_types.h"

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif

/*
 * Set all the bits in tgt to random values.  The number of 
 * words set comes from mdp. 
 */
int qrlbn_random_bit_vec(uint32_t *tgt,
			 const qrlbn_modulus_data_t *mdp);


/*
 * Returns a random bignum r, such that minval <= r < modulus.
 * Think of it as an element of the field or group 
 * associated with mdp, except the caller is resposible for the
 * minimum value: 0 for an additive group, 1 for a multiplicative
 * group.
 */ 
int qrlbn_random_element(uint32_t *tgt, uint32_t minval, 
			 const qrlbn_modulus_data_t *mdp);


/*
 * Returns an unsigned integer between min and max, inclusive.  All values
 * exactly equal, up to the quality of the underlying source
 * of randomness.  If min > max, they are swapped.
 */
uint32_t qrlbn_random_range(uint32_t min, uint32_t max);

#ifdef	__cplusplus
}
#endif


#endif
