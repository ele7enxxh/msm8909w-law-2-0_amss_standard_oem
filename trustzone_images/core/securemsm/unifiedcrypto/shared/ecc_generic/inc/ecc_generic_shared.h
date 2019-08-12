#ifndef _GEN_ECC_SHARED_H
#define _GEN_ECC_SHARED_H

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

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/unifiedcrypto/shared/ecc_generic/inc/ecc_generic_shared.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

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
 * @Initializes a domain from curve hex strings and the cofactor.
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
SW_GENERIC_ECC_init(qrlbn_ecc_domain_t * dp, char *modulus, char *a, 
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
SW_GENERIC_ECDSA_sign(uint8_t *msgdgst, 
                      uint32_t msgdgst_len, 
      	              qrlbn_ecc_bigval_t *privkey,                                   
                      qrlbn_ECDSA_sig_t *  sig,
                      qrlbn_ecc_domain_t *dp); 

/**
 * @brief verify data with ECC public key
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
SW_GENERIC_ECDSA_verify(uint8_t *msgdgst, 
                        uint32_t msgdgst_len, 
                        qrlbn_ecc_affine_point_t *pubkey, 
                        qrlbn_ECDSA_sig_t *sig,
                        qrlbn_ecc_domain_t *dp);

/**
 * @brief generate shared key out of Alice' public key and Bob's private key
 *
 * @param shared_key   [out] shared key bewteen Alice and Bob
 * @param pubkey       [in]  pointer to ECC public key
 * @param privkey      [in]  pointer to ECC private key 
 * @param dp           [in]  ECC domain context 
 *
 * @return sw_crypto_errno_enum_type
 *
 * @see 
 *
 */

sw_crypto_errno_enum_type 
SW_GENERIC_ECDH_shared_key_derive(qrlbn_ecc_bigval_t *shared_key, 
                                  qrlbn_ecc_bigval_t *privkey, 
                                  qrlbn_ecc_affine_point_t *pubkey, 
                                  qrlbn_ecc_domain_t *dp);

/**
 * @brief generate public key and private key of ECC
 *
 * @param privkey              [out] pointer to private key 
 * @param pubkey               [out] pointer to public key
 * @param dp                   [in]  ECC domain context
 *
 * @return sw_crypto_errno_enum_typek
 *
 * @see 
 *
 */
 sw_crypto_errno_enum_type
SW_GENERIC_ECC_keypair_generate(qrlbn_ecc_bigval_t *privkey, 
		                        qrlbn_ecc_affine_point_t  *pubkey, 
  	                            qrlbn_ecc_domain_t *dp);

 /**
 * @brief Convert data from Binary to Bigval
 *
 * @param dst              [out] pointer to destination data 
 * @param src              [in]  pointer to source data
 * @param srclen           [in]  length of source data in bytes 
 * @param dp               [in]  ECC domain context 
 * @param tag              [in]  Data type tag
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_binary_to_bigval(qrlbn_ecc_bigval_t *dst, 
                                const void *src, 
                                size_t srclen,
		                        const qrlbn_ecc_domain_t *dp, 
                                qrlbn_field_tag_t tag);

/**
 * @brief Convert data from Bigval to Binary
 *
 * @param dst              [out] pointer to destination data 
 * @param dstlen           [in]  length of destination data in bytes 
 * @param src              [in]  pointer to source data
 * @param dp               [in]  ECC domain context 
 * @param tag              [in]  Data type tag
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_bigval_to_binary(uint8_t *dst, 
                                size_t dstlen,
                                const qrlbn_ecc_bigval_t *src, 
		                        const qrlbn_ecc_domain_t *dp, 
                                qrlbn_field_tag_t tag);


/**
 * @brief Compare mem. It is not constant time comparison and intended for only testing purpose
 *
 * @param a              [in]  pointer to data
 * @param b              [in]  pointer to data
 * @param mdp            [in]  ECC modulus data
 *
 * @return sw_crypto_errno_enum_type
 *
 */
sw_crypto_errno_enum_type
SW_GENERIC_ECC_compare(const uint32_t *a, 
                       const uint32_t *b, 
                       const qrlbn_modulus_data_t *mdp);

#endif /* _GEN_ECC_SHARED_H */
