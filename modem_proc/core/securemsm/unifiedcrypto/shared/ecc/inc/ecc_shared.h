#ifndef _ECC_SHARED_H
#define _ECC_SHARED_H

/*===========================================================================

                             Unified Crypto 

GENERAL DESCRIPTION
Provide ECDSA cryptography. It supports flexible key size with different curves.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/unifiedcrypto/shared/ecc/inc/ecc_shared.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/13   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uc_env.h"
#include "uc_comdef.h"
#include "qrlbn_ecc_types.h"
#include "qrlbn_ecc.h"
#include "qrlbn_arith.h"

 /**
 * @Initializes a domain from hex strings and the cofactor.
 *
 * @param dp             [out] pointer to ECC domain context
 * @param modulus        [in]  pointer to modulus 
 * @param a              [in]  pointer to a 
 * @param b              [in]  pointer to b  
 * @param x              [in]  pointer to x  
 * @param y              [in]  pointer to y   
 * @param n              [in]  pointer to n    
 * @param cofactor       [in] cofactor    
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type 
SW_ECC_init(qrlbn_ecc_domain_t * dp, char *modulus, char *a, 
	        char* b, char *x, char *y, char *n, unsigned cofactor);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param privkey           [in]  pointer to private key for signing
 * @param sig               [out] pointer to signed msg, which is signature
 * @param sig_len           [out] signature size in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type
SW_ECDSA_Sign(uint8_t *msgdgst, 
              uint32_t msgdgst_len, 
      	      qrlbn_ecc_bigval_t *privkey,                                   
              qrlbn_ECDSA_sig_t *  sig,
              qrlbn_ecc_domain_t *dp);

/**
 * @brief sign data with ECC private key
 *
 * @param msgdgst           [in]  pointer to msg that needs to be signed
 * @param msgdgst_len       [in]  msg length in Bytes
 * @param pubkey            [in]  pointer to public key for verification
 * @param sig               [out] pointer to signed msg to be verfied
 * @param sig_len           [out] sig_len in Bytes
 * @param dp                [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type
SW_ECDSA_Verify(uint8_t *msgdgst, 
                uint32_t msgdgst_len, 
                qrlbn_ecc_affine_point_t *pubkey, 
                qrlbn_ECDSA_sig_t *sig,
                qrlbn_ecc_domain_t *dp);

/**
 * @brief sign data with ECC private key
 *
 * @param k                    [out] pointer to private key
 * @param P                    [out] pointer to public key
 * @param dp                   [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_typek
 *
 * @see 
 *
 */
 sw_crypto_errno_enum_type
SW_ECC_KeyGenerate(qrlbn_ecc_bigval_t *  k, 
		           qrlbn_ecc_affine_point_t  *  P, 
  	               qrlbn_ecc_domain_t *dp);

#endif /* _ECC_SHARED_H */
