#ifndef _QRLBN_ECC_OPS_H
#define _QRLBN_ECC_OPS_H

/*

  Copyright © 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  QUALCOMM Confidential and Proprietary
  
*/

#include <stdint.h>
#include "qrlbn_arith.h"
#include "qrlbn_ecc_types.h"

#ifdef	__cplusplus
extern "C" {
#endif
#if 0
} /* keep emacs from indenting all the stuff below */
#endif


/*
 * p must be non-negative.  p and tgt can alias
 */
int qrlbn_point_to_affine(qrlbn_ecc_point_t *tgt, const qrlbn_ecc_point_t *p, 
			  const qrlbn_ecc_domain_t *dp);



/*
 * coordinates of p must be non-negative
 * z and p can alias
 */

int qrlbn_point_dbl(qrlbn_ecc_point_t *z, const qrlbn_ecc_point_t *p, 
			const qrlbn_ecc_domain_t *dp);


/*
 * coordinates of p and q must be non-negative
 * z, p, and q can alias
 */
int qrlbn_point_add(qrlbn_ecc_point_t *z, const qrlbn_ecc_point_t *p, 
		  const qrlbn_ecc_point_t *q, 
		  const qrlbn_ecc_domain_t * dp);

 

int qrlbn_point_multiply(qrlbn_ecc_point_t *  z, 
			 const qrlbn_ecc_bigval_t *k, 
			 const qrlbn_ecc_point_t *base, 
			 const qrlbn_ecc_domain_t *dp, int flags);

int qrlbn_import_point(qrlbn_ecc_point_t *  tgt, 
		       const qrlbn_ecc_affine_point_t *p,
		       const qrlbn_ecc_domain_t *dp);

int qrlbn_export_point(qrlbn_ecc_affine_point_t *  tgt,
		       const qrlbn_ecc_point_t *p, 
		       const qrlbn_ecc_domain_t *dp);

boolean_t
qrlbn_point_on_curve(const qrlbn_ecc_point_t *Q, 
		     const qrlbn_ecc_domain_t *dp);


#ifdef	__cplusplus
}
#endif

#endif
